/*
 * cmd_processing.cpp
 *
 *  Created on: 07.06.2015
 *      Author: schmelly
 */

#include "cmd_processing.h"

#include <AH_Pololu.h>
#include <stdint.h>
#include <stdio.h>

#include "cmd_handling.h"
#include "config.h"
#include "fixedpoint_math.h"
#include "galvo_math.h"
#include "spi.h"
#include "stepper.h"

void handleLinearMove();
void handleZMove();
void handleLaser();
void linearMoveHelper(int32_t sX, int32_t sY, int32_t stepsPerSegment);

float oldX = 0.0f, oldY = 0.0f;

int processCommand(enum commands cmd) {

  int result = 0;

  switch (cmd) {
  case LINEAR_MOVE:
    handleLinearMove();
    result = 1;
    break;
  case Z_MOVE:
    handleZMove();
    result = 1;
    break;
  case LASER:
    handleLaser();
    result = 1;
    break;
  case ARBITRARY_GCODE:
    result = 1;
    break;
  default:
    break;
  }

  return result;
}

int32_t old_sX;
int32_t old_sY;
int32_t old_dX;
int32_t old_dY;

int32_t dots;

void handleLinearMove() {

  float* xy = getXYCoords();

  // #########################################

  int32_t x1 = oldX;
  int32_t y1 = oldY;
  int32_t x2 = float2Fixed(xy[0]);
  int32_t y2 = float2Fixed(xy[1]);

  char msg[100];
  memset(msg, 0, sizeof(char) * 100);
  sprintf(msg, "from (%f,%f) to (%f,%f)", fixed2Float(x1), fixed2Float(y1), fixed2Float(x2), fixed2Float(y2));
  Serial.println(msg);

  dots = 0;
  uint32_t start = micros();

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

  //changeLaserValue(196);

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

  oldX = x2;
  oldY = y2;

  //changeLaserValue(0);

  // #########################################

  uint32_t stop = micros();
  uint32_t time = stop - start;

  memset(msg, 0, sizeof(char) * 100);
  sprintf(msg, "%ld micros total, %ld dots total, %ld micros per dot", time, dots, time / dots);
  Serial.println(msg);
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
      dots++;
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

void handleZMove() {

  float zCoord = getZCoord();
  int steps = (int) (STEPS_PER_MM_Z * zCoord);

  Stepper.sleepOFF();
  Stepper.move(steps);
  Stepper.sleepON();
}

void handleLaser() {

  int value = getLaser();
  changeLaserValue(value);
}
