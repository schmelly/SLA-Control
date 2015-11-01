/*
 * cmd_processing.cpp
 *
 *  Created on: 07.06.2015
 *      Author: schmelly
 */

#include "cmd_processing.h"

#include <AH_Pololu.h>
#include <float.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>

//#include "cmd_handling.h"
#include "config.h"
#include "fixedpoint_math.h"
#include "galvo_math.h"
#include "spi.h"
#include "stepper.h"

void handleMove(struct gCode* code);
void handlePrintingMove(struct gCode* code);
void handleNonPrintingMove(struct gCode* code);
void handleZMove(struct gCode* code);
void handleLaser(struct gCode* code);
void linearMoveHelper(int32_t sX, int32_t sY, int32_t stepsPerSegment);

int laser_intensity = 0;
float oldX = 0.0f, oldY = 0.0f;

void processCommand(struct gCode* code) {

  //printCode(code);

  switch (code->code) {
  case G0:
  case G1:
    handleMove(code);
    handleZMove(code);
    Serial.println("ok");
    break;
  case L:
    handleLaser(code);
    Serial.println("ok");
    break;
  case S:
    if (code->switchLaser >= 0) {
      changeLaserValue(laser_intensity);
    } else {
      changeLaserValue(0);
    }
    Serial.println("ok");
    break;
  case M105:
    Serial.println("ok T:0 B:0");
    break;
  case M110:
  case UNKNOWN:
    Serial.println("ok");
    break;
  }
}

int32_t old_sX;
int32_t old_sY;
int32_t old_dX;
int32_t old_dY;

//int32_t dots;

void handleMove(struct gCode* code) {

  if (code->xCoord == -FLT_MAX || code->yCoord == -FLT_MAX) {
    return;
  }

  if (code->eCoord > 0) {
    handlePrintingMove(code);
  } else {
    handleNonPrintingMove(code);
  }
}

void handlePrintingMove(struct gCode* code) {

  float xy[2]; //getXYCoords();
  xy[0] = code->xCoord;
  xy[1] = code->yCoord;

  int32_t x1 = oldX;
  int32_t y1 = oldY;
  int32_t x2 = float2Fixed(xy[0]);
  int32_t y2 = float2Fixed(xy[1]);

  char msg[100];
  memset(msg, '\0', sizeof(char) * 100);
  sprintf(msg, "printing from (%f,%f) to (%f,%f)", fixed2Float(x1), fixed2Float(y1), fixed2Float(x2), fixed2Float(y2));
  Serial.println(msg);

//  dots = 0;
//  uint32_t start = micros();

  int32_t tmp1 = sub32(x1, x2); // (x1 - x2)
  int32_t tmp2 = sub32(y1, y2); // (y1 - y2)
  int32_t tmp3 = sq32(tmp1); // (x1 - x2)^2
  int32_t tmp4 = sq32(tmp2); // (y1 - y2)^2

  //float length = sqrt((x1 - x2)^2 + (y1 - y2)^2));
  int32_t length = sqrt32(add32(tmp3, tmp4));

  //float segmentRemainder = fmod(length, SEGMENT_LENGTH);
  int32_t segmentRemainder = length % SEGMENT_LENGTH;

  int32_t tmp5 = div32(segmentRemainder, length); // (segmentRemainder / length)
  int32_t tmp6 = sub32(x2, x1); // (x2 - x1)
  int32_t tmp7 = sub32(y2, y1); // (y2 - y1)
  int32_t tmp8 = mul32(tmp6, tmp5); // (x2 - x1) * (segmentRemainder / length)
  int32_t tmp9 = mul32(tmp7, tmp5); // (y2 - y1) * (segmentRemainder / length)

  //float x2_ = x2 - (x2 - x1) * (segmentRemainder / length);
  int32_t x2_ = sub32(x2, tmp8);
  //float y2_ = y2 - (y2 - y1) * (segmentRemainder / length);
  int32_t y2_ = sub32(y2, tmp9);

  //int numSegments = length / SEGMENT_LENGTH;
  int32_t numSegments = div32(length, SEGMENT_LENGTH);

  //int numSegmentsCoords = (numSegments) * 2;
  int32_t numSegmentsCoords = (int32_t) fixed2Float(numSegments) * 2;
  int32_t segmentCoords[numSegmentsCoords];

  int32_t i, j = 0;
  for (i = 0; i < numSegmentsCoords; i += 2) {

    j++;

    int32_t tmp10 = sub32(x2_, x1); // (x2_ - x1)
    int32_t tmp11 = sub32(y2_, y1); // (y2_ - y1)
    int32_t tmp12 = mul32(float2Fixed(j), tmp10); // j * (x2_ - x1)
    int32_t tmp13 = mul32(float2Fixed(j), tmp11); // j * (y2_ - y1)

    //segmentCoords[i] = x1 + j * (x2_ - x1) / numSegments;
    segmentCoords[i] = add32(x1, div32(tmp12, numSegments));
    //segmentCoords[i + 1] = y1 + j * (y2_ - y1) / numSegments;
    segmentCoords[i + 1] = add32(y1, div32(tmp13, numSegments));
  }

  //first part: process each segment
  old_sX = x1;
  old_sY = y1;
  old_dX = xyToAlphaDigit(old_sX, old_sY);
  old_dY = yToBetaDigit(old_sY);

  changeLaserValue(laser_intensity);

  for (i = 0; i < numSegmentsCoords; i += 2) {

    int32_t sX = segmentCoords[i];
    int32_t sY = segmentCoords[i + 1];

    linearMoveHelper(sX, sY, MAX_STEPS_PER_SEGMENT);

    old_sX = sX;
    old_sY = sY;
  }

  //2. step last segment to endpoint
  if (segmentRemainder != 0) {

    int32_t stepsPerSegment = mul32(MAX_STEPS_PER_SEGMENT, div32(segmentRemainder, length));
    linearMoveHelper(x2, y2, stepsPerSegment);
  }

  changeLaserValue(0);
  delay(1);

  oldX = x2;
  oldY = y2;

//  uint32_t stop = micros();
//  uint32_t time = stop - start;
//
//  memset(msg, 0, sizeof(char) * 100);
//  sprintf(msg, "%ld micros total, %ld dots total, %ld micros per dot", time, dots, time / dots);
//  Serial.println(msg);
}

void handleNonPrintingMove(struct gCode* code) {

  int32_t x = float2Fixed(code->xCoord);
  int32_t y = float2Fixed(code->yCoord);

  int32_t dX = xyToAlphaDigit(x, y);
  int32_t dY = yToBetaDigit(y);

  changeMotorValues(dX, dY);
  delay(1);

  char msg[100];
  memset(msg, '\0', sizeof(char) * 100);
  sprintf(msg, "moving from (%f,%f) to (%f,%f)", fixed2Float(oldX), fixed2Float(oldY), fixed2Float(x), fixed2Float(y));
  Serial.println(msg);

  oldX = x;
  oldY = y;
}

void linearMoveHelper(int32_t sX, int32_t sY, int32_t stepsPerSegment) {

  int32_t i;

  uint32_t calcStart;
  int32_t waitTime;

  for (i = 0; i < fixed2Float(stepsPerSegment); i++) {

    calcStart = micros();

    int32_t tmp1 = sub32(sX, old_sX); // (sX - old_sX)
    int32_t tmp2 = sub32(sY, old_sY); // (sY - old_sY)
    int32_t tmp3 = mul32(float2Fixed(i), div32(tmp1, stepsPerSegment)); // i * (sX - old_sX) / stepsPerSegment
    int32_t tmp4 = mul32(float2Fixed(i), div32(tmp2, stepsPerSegment)); // (sY - old_sY) / stepsPerSegment

    //float sX_ = old_sX + i * (sX - old_sX) / stepsPerSegment;
    int32_t sX_ = add32(old_sX, tmp3);
    //float sY_ = old_sY + i * (sY - old_sY) / stepsPerSegment;
    int32_t sY_ = add32(old_sY, tmp4);

    int32_t dX = xyToAlphaDigit(sX_, sY_);
    int32_t dY = yToBetaDigit(sY_);

    if (old_dX != dX || old_dY != dY) {
      //changeMotorValues(dX, dY);
      //printf("%d|%d\n", dX, dY);
      //dots++;
    }

    old_dX = dX;
    old_dY = dY;

    //calculate the exposure time for a single spot
    waitTime = (int32_t)(EXPOSURE_TIME - (micros() - calcStart));

    if (waitTime > 0) {
      delayMicroseconds(waitTime);
    }
  }
}

void handleZMove(struct gCode* code) {

  if (code->zCoord == -FLT_MAX) {
    return;
  }

  float zCoord = code->zCoord;
  int steps = (int) (STEPS_PER_MM_Z * zCoord);

  Stepper.sleepOFF();
  Stepper.move(steps);
  Stepper.sleepON();

  char msg[100];
  memset(msg, '\0', sizeof(char) * 100);
  sprintf(msg, "zMove: %f mm | %d steps", zCoord, steps);
  Serial.println(msg);
}

void handleLaser(struct gCode* code) {

  if (code->laserIntensity == INT_MIN) {
    return;
  }

  laser_intensity = code->laserIntensity;
  //int value = code->laserIntensity;
  //changeLaserValue(value);

  char msg[100];
  memset(msg, '\0', sizeof(char) * 100);
  sprintf(msg, "laser intensity: %d", laser_intensity);
  Serial.println(msg);
}
