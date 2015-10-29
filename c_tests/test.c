/*
 * test.c
 *
 *  Created on: 06.04.2015
 *      Author: schmelly
 */

#include "config.h"
#include "galvo_math.h"
#include "fixedpoint_math.h"
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void motorValuesOld() {

  float xy[2];
  xy[0] = 0.f;
  xy[1] = 10000.f;

  float oldX = 0.f;
  float oldY = 0.f;

  float x1 = oldX;
  float y1 = oldY;
  float x2 = xy[0];
  float y2 = xy[1];

  float length = (sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)));
  float segmentRemainder = fmod(length, SEGMENT_LENGTH);

  float x2_ = x2 - (x2 - x1) * (segmentRemainder / length);
  float y2_ = y2 - (y2 - y1) * (segmentRemainder / length);

  int numSegments = length / SEGMENT_LENGTH;
  int numSegmentsCoords = (numSegments) * 2;

  float segmentCoords[numSegmentsCoords];

  int i, j = 0;
  for (i = 0; i < numSegmentsCoords; i += 2) {
    j++;
    segmentCoords[i] = x1 + j * (x2_ - x1) / numSegments;
    segmentCoords[i + 1] = y1 + j * (y2_ - y1) / numSegments;
  }

  //first part: process each segment
  float old_sX = x1;
  float old_sY = y1;
  int old_dX = xyToAlphaDigit(old_sX, old_sY);
  int old_dY = yToBetaDigit(old_sY);

  int coords = 0;
  int doubleValues = 0;

  //changeLaserValue(196);

  for (i = 0; i < numSegmentsCoords; i += 2) {

    float sX = segmentCoords[i];
    float sY = segmentCoords[i + 1];

    for (j = 0; j < MAX_STEPS_PER_SEGMENT; j++) {
      float sX_ = old_sX + j * (sX - old_sX) / MAX_STEPS_PER_SEGMENT;
      float sY_ = old_sY + j * (sY - old_sY) / MAX_STEPS_PER_SEGMENT;

      int dX = xyToAlphaDigit(sX_, sY_);
      int dY = yToBetaDigit(sY_);

      if (old_dX != dX || old_dY != dY) {
        coords++;
        //changeMotorValues(dX, dY);
        printf("dX: %d | dY: %d\n", dX, dY);
      } else {
        doubleValues++;
      }

      old_dX = dX;
      old_dY = dY;
    }

    old_sX = sX;
    old_sY = sY;
  }

  //2. step last segment to endpoint
  if (segmentRemainder != 0) {
    for (j = 0; j < MAX_STEPS_PER_SEGMENT * (segmentRemainder / length); j++) {
      float sX_ = old_sX + j * (x2 - old_sX) / (MAX_STEPS_PER_SEGMENT * (segmentRemainder / length));
      float sY_ = old_sY + j * (y2 - old_sY) / (MAX_STEPS_PER_SEGMENT * (segmentRemainder / length));

      int dX = xyToAlphaDigit(sX_, sY_);
      int dY = yToBetaDigit(sY_);

      if (old_dX != dX || old_dY != dY) {
        coords++;
        //changeMotorValues(dX, dY);
        printf("dX: %d | dY: %d\n", dX, dY);
      } else {
        doubleValues++;
      }

      old_dX = dX;
      old_dY = dY;
    }
  }

  oldX = x2;
  oldY = y2;

  //changeLaserValue(0);
}

int32_t old_sX;
int32_t old_sY;
int32_t old_dX;
int32_t old_dY;

void moveMotor(int32_t sX, int32_t sY, int32_t numSegmentsCoords, int32_t* segmentCoords, int32_t stepsPerSegment) {

  int32_t i;

  for (i = 0; i < fixed2Float(stepsPerSegment); i++) {
    float sX_ = old_sX + i * (sX - old_sX) / stepsPerSegment;
    float sY_ = old_sY + i * (sY - old_sY) / stepsPerSegment;

    int dX = xyToAlphaDigit(sX_, sY_);
    int dY = yToBetaDigit(sY_);

    if (old_dX != dX || old_dY != dY) {
      //changeMotorValues(dX, dY);
      printf("dX: %d | dY: %d\n", dX, dY);
    }

    old_dX = dX;
    old_dY = dY;
  }
}

void motorValuesNew() {

  float xy[2];
  xy[0] = 0.f;
  xy[1] = 10000.f;

  int32_t oldX = float2Fixed(0.f);
  int32_t oldY = float2Fixed(0.f);

  int32_t x1 = oldX;
  int32_t y1 = oldY;
  int32_t x2 = xy[0];
  int32_t y2 = xy[1];

  int32_t tmp1 = q_sub(x1, x2);
  int32_t tmp2 = q_sub(y1, y2);
  int32_t tmp3 = q_sq(tmp1);
  int32_t tmp4 = q_sq(tmp2);

  //float length = (sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)));
  //float segmentRemainder = fmod(length, SEGMENT_LENGTH);

  // TODO: fixed point sqrt
  int32_t length = float2Fixed(sqrt(fixed2Float(q_add(tmp3, tmp4))));
  //TODO: fixed point fmod
  int32_t segmentRemainder = float2Fixed(fmod(fixed2Float(length), fixed2Float(SEGMENT_LENGTH)));

  int32_t tmp5 = q_div(segmentRemainder, length);
  int32_t tmp6 = q_sub(x2, x1);
  int32_t tmp7 = q_sub(y2, y1);
  int32_t tmp8 = q_mul(tmp6, tmp5);
  int32_t tmp9 = q_mul(tmp7, tmp5);

  //float x2_ = x2 - (x2 - x1) * (segmentRemainder / length);
  //float y2_ = y2 - (y2 - y1) * (segmentRemainder / length);

  int32_t x2_ = q_sub(x2, tmp8);
  int32_t y2_ = q_sub(y2, tmp9);

  //int numSegments = length / SEGMENT_LENGTH;
  //int numSegmentsCoords = (numSegments) * 2;

  int32_t numSegments = q_div(length, SEGMENT_LENGTH);
  int32_t numSegmentsCoords = (int32_t) fixed2Float(numSegments) * 2;

  //float segmentCoords[numSegmentsCoords];

  int32_t segmentCoords[numSegmentsCoords];

  int32_t i, j = 0;
  for (i = 0; i < numSegmentsCoords; i += 2) {
    j++;
    //segmentCoords[i] = x1 + j * (x2_ - x1) / numSegments;
    //segmentCoords[i + 1] = y1 + j * (y2_ - y1) / numSegments;

    int32_t tmp10 = q_sub(x2_, x1);
    int32_t tmp11 = q_sub(y2_, y1);
    int32_t tmp12 = q_mul(float2Fixed(j), tmp10);
    int32_t tmp13 = q_mul(float2Fixed(j), tmp11);

    segmentCoords[i] = q_add(x1, q_div(tmp12, numSegments));
    segmentCoords[i + 1] = q_add(y1, q_div(tmp13, numSegments));
  }

  //first part: process each segment
  //snip

  // /snip
  old_sX = x1;
  old_sY = y1;
  old_dX = xyToAlphaDigit(old_sX, old_sY);
  old_dY = yToBetaDigit(old_sY);

  //changeLaserValue(196);
  for (i = 0; i < numSegmentsCoords; i += 2) {

    int32_t sX = segmentCoords[i];
    int32_t sY = segmentCoords[i + 1];

    moveMotor(sX, sY, numSegmentsCoords, segmentCoords, MAX_STEPS_PER_SEGMENT);

    old_sX = sX;
    old_sY = sY;
  }

  //2. step last segment to endpoint
  if (segmentRemainder != 0) {
    for (j = 0; j < MAX_STEPS_PER_SEGMENT * (segmentRemainder / length); j++) {
      float sX_ = old_sX + j * (x2 - old_sX) / (MAX_STEPS_PER_SEGMENT * (segmentRemainder / length));
      float sY_ = old_sY + j * (y2 - old_sY) / (MAX_STEPS_PER_SEGMENT * (segmentRemainder / length));

      int dX = xyToAlphaDigit(sX_, sY_);
      int dY = yToBetaDigit(sY_);

      if (old_dX != dX || old_dY != dY) {
        //changeMotorValues(dX, dY);
        printf("dX: %d | dY: %d\n", dX, dY);
      }

      old_dX = dX;
      old_dY = dY;
    }
  }

  oldX = x2;
  oldY = y2;

  //changeLaserValue(0);
}

int main(int argc, char **argv) {

  //setvbuf(stdout, NULL, _IONBF, 0);
  //setvbuf(stderr, NULL, _IONBF, 0);

  motorValuesOld();
  printf("----------------------------\n");
  motorValuesNew();

  return 0;
}

//        fprintf(fp, "Position: %.3f,%.3f\n", sX_, sY_);
//        fprintf(fp, "Digital:  %d,%d\n", dX, dY);
//        fprintf(fp, "Winkel:   %.3f,%.3f\n", digitToAlpha(dX), digitToBeta(dY));
//        fprintf(fp, "\n");
