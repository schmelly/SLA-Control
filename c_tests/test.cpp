/*
 * test.c
 *
 *  Created on: 06.04.2015
 *      Author: schmelly
 */

#include "config.h"
#include "galvo_math.h"
#include "fixedpoint_math.h"
#include "gcode_parser.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

//void motorValuesOld() {
//
//  float xy[2];
//  xy[0] = 75.f;
//  xy[1] = 0.f;
//
//  float oldX = -75.f;
//  float oldY = 0.f;
//
//  float x1 = oldX;
//  float y1 = oldY;
//  float x2 = xy[0];
//  float y2 = xy[1];
//
//  float length = (sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)));
//  float segmentRemainder = fmod(length, fixed2Float(SEGMENT_LENGTH));
//
//  float x2_ = x2 - (x2 - x1) * (segmentRemainder / length);
//  float y2_ = y2 - (y2 - y1) * (segmentRemainder / length);
//
//  int numSegments = length / fixed2Float(SEGMENT_LENGTH);
//  int numSegmentsCoords = (numSegments) * 2;
//
//  float segmentCoords[numSegmentsCoords];
//
//  int i, j = 0;
//  for (i = 0; i < numSegmentsCoords; i += 2) {
//    j++;
//    segmentCoords[i] = x1 + j * (x2_ - x1) / numSegments;
//    segmentCoords[i + 1] = y1 + j * (y2_ - y1) / numSegments;
//  }
//
//  //first part: process each segment
//  float old_sX = x1;
//  float old_sY = y1;
//  int old_dX = xyToAlphaDigit(old_sX, old_sY);
//  int old_dY = yToBetaDigit(old_sY);
//
//  int coords = 0;
//  int doubleValues = 0;
//
//  //changeLaserValue(196);
//
//  for (i = 0; i < numSegmentsCoords; i += 2) {
//
//    float sX = segmentCoords[i];
//    float sY = segmentCoords[i + 1];
//
//    for (j = 0; j < fixed2Float(MAX_STEPS_PER_SEGMENT); j++) {
//      float sX_ = old_sX + j * (sX - old_sX) / fixed2Float(MAX_STEPS_PER_SEGMENT);
//      float sY_ = old_sY + j * (sY - old_sY) / fixed2Float(MAX_STEPS_PER_SEGMENT);
//
//      int dX = xyToAlphaDigit(sX_, sY_);
//      int dY = yToBetaDigit(sY_);
//
//      if (old_dX != dX || old_dY != dY) {
//        coords++;
//        //changeMotorValues(dX, dY);
//        printf("%d|%d\n", dX, dY);
//      } else {
//        doubleValues++;
//      }
//
//      old_dX = dX;
//      old_dY = dY;
//    }
//
//    old_sX = sX;
//    old_sY = sY;
//  }
//
//  //2. step last segment to endpoint
//  if (segmentRemainder != 0) {
//    for (j = 0; j < fixed2Float(MAX_STEPS_PER_SEGMENT) * (segmentRemainder / length); j++) {
//      float sX_ = old_sX + j * (x2 - old_sX) / (fixed2Float(MAX_STEPS_PER_SEGMENT) * (segmentRemainder / length));
//      float sY_ = old_sY + j * (y2 - old_sY) / (fixed2Float(MAX_STEPS_PER_SEGMENT) * (segmentRemainder / length));
//
//      int dX = xyToAlphaDigit(sX_, sY_);
//      int dY = yToBetaDigit(sY_);
//
//      if (old_dX != dX || old_dY != dY) {
//        coords++;
//        //changeMotorValues(dX, dY);
//        printf("%d|%d\n", dX, dY);
//      } else {
//        doubleValues++;
//      }
//
//      old_dX = dX;
//      old_dY = dY;
//    }
//  }
//
//  oldX = x2;
//  oldY = y2;
//
//  //changeLaserValue(0);
//}

int32_t old_sX;
int32_t old_sY;
int32_t old_dX;
int32_t old_dY;

int32_t duplicates = 0;

void moveMotor(int32_t sX, int32_t sY, int32_t stepsPerSegment) {

  int32_t i;

  for (i = 0; i < fixed2Float(stepsPerSegment); i++) {

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
      printf("%d|%d\n", dX, dY);
    } else {
      duplicates++;
    }

    old_dX = dX;
    old_dY = dY;
  }
}

void motorValuesNew() {

  float xy[2];
  xy[0] = 75.f;
  xy[1] = 75.f;

  int32_t oldX = float2Fixed(-75.f);
  int32_t oldY = float2Fixed(-75.f);

  int32_t x1 = oldX;
  int32_t y1 = oldY;
  int32_t x2 = float2Fixed(xy[0]);
  int32_t y2 = float2Fixed(xy[1]);

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

    moveMotor(sX, sY, MAX_STEPS_PER_SEGMENT);

    old_sX = sX;
    old_sY = sY;
  }

  //2. step last segment to endpoint
  if (segmentRemainder != 0) {

    int32_t stepsPerSegment = mul32(MAX_STEPS_PER_SEGMENT, div32(segmentRemainder, length));
    moveMotor(x2, y2, stepsPerSegment);
  }

  oldX = x2;
  oldY = y2;

  //changeLaserValue(0);
}

void printCode(gCode* code) {

  printf("gCode: %d, xCoord: %f, yCoord: %f, zCoord: %f, eCoord: %f, fCoord: %f, lineNo: %d\n", code->code, code->xCoord, code->yCoord, code->zCoord, code->eCoord, code->fCoord, code->lineNo);
}

int main(int argc, char **argv) {

  char* line = "; Kommentar N123 G1 X-1.234 Y0.567 E5 \r";
  gCode* code = parseGCodeLine(line);
  printCode(code);

  printf("%d %d\n", code->fCoord == FLT_MIN, code->fCoord == 0.f);

  fflush (stdout);

//  int i;
//
//  for (i = 0; i < 50; i++) {
//    a += 16;
//
//    if ((int8_t)(a - wait) >= 0) {
//      printf("1 | %d | %d | should do something\n", a, wait);
//      wait += 64;
//    } else {
//      printf("0 | %d | %d | waiting\n", a, wait);
//    }
//  }

  //motorValuesNew();
  //printf("duplicates: %d", duplicates);

  // ...
  // calculate digits
  // move galvos
  // wait (should be lit for x ms now)
  // calculate digits
  //
  // wait

  return 0;
}
