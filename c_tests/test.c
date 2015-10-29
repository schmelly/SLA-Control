/*
 * test.c
 *
 *  Created on: 06.04.2015
 *      Author: schmelly
 */

#include "math.h"
#include "config.h"
#include "galvo_math.h"
#include "fixedpoint_math.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int main(int argc, char **argv) {

  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);

  float x = -75.f;
  float y = -75.f;

  int i;
  for (i = 0; i < 30; i++) {

    int32_t alphaDigit_f = xyToAlphaDigit(x, y);
    float alphaDigit = xyToAlphaDigit2(x, y);

    int32_t betaDigit_f = yToBetaDigit(y);
    float betaDigit = yToBetaDigit2(y);

    printf("xyToAlphaDigit(%f, %f): %d | yToBetaDigit(%f): %d\n", x, y, alphaDigit_f, y, betaDigit_f);
    printf("xyToAlphaDigit2(%f, %f): %f | yToBetaDigit2(%f): %f\n", x, y, alphaDigit, y, betaDigit);
    printf("error alpha: %d | error beta: %d\n", abs(alphaDigit_f - alphaDigit), abs(betaDigit_f - betaDigit));
    printf("------------------------------\n");

    x += 5.f;
    y += 5.f;
  }

  /**
   * 1. calculate line segment positions
   * 2. for each segment:
   *   a. calculate start/endpoint angles
   *   b. interpolate linear
   */

//  float x1 = 0.0f;
//  float y1 = 0.0f;
//  float x2 = 10000.0f;
//  float y2 = 0.0f;
//
//  float length = (sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)));
//  float segmentRemainder = fmod(length, SEGMENT_LENGTH);
//
//  float x2_ = x2 - (x2 - x1) * (segmentRemainder / length);
//  float y2_ = y2 - (y2 - y1) * (segmentRemainder / length);
//
//  int numSegments = length / SEGMENT_LENGTH;
//  int numSegmentsCoords = (numSegments) * 2;
//
//  float segmentCoords[numSegmentsCoords];
//
//  printf("length(microns): %.3f segmentRemainder: %.3f num_segments: %d numSegmentsCoords %d x2_: %.3f y2_: %.3f\n", length, segmentRemainder, numSegments, numSegmentsCoords, x2_, y2_);
//
//  int i, j = 0;
//  for (i = 0; i < numSegmentsCoords; i += 2) {
//    j++;
//    segmentCoords[i] = x1 + j * (x2_ - x1) / numSegments;
//    segmentCoords[i + 1] = y1 + j * (y2_ - y1) / numSegments;
//  }
//
//  FILE *fp;
//
//  fp = fopen("./out.txt", "w+");
//  fprintf(fp, "This is testing for fprintf...\n");
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
//  for (i = 0; i < numSegmentsCoords; i += 2) {
//
//    float sX = segmentCoords[i];
//    float sY = segmentCoords[i + 1];
//
//    for (j = 0; j < MAX_STEPS_PER_SEGMENT; j++) {
//      float sX_ = old_sX + j * (sX - old_sX) / MAX_STEPS_PER_SEGMENT;
//      float sY_ = old_sY + j * (sY - old_sY) / MAX_STEPS_PER_SEGMENT;
//
//      int dX = xyToAlphaDigit(sX_, sY_);
//      int dY = yToBetaDigit(sY_);
//
//      if (old_dX != dX || old_dY != dY) {
//        fprintf(fp, "[%.5f,%.5f],", digitToAlpha(dX), digitToBeta(dY));
//        coords++;
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
//    for (j = 0; j < MAX_STEPS_PER_SEGMENT * (segmentRemainder / length); j++) {
//      float sX_ = old_sX + j * (x2 - old_sX) / (MAX_STEPS_PER_SEGMENT * (segmentRemainder / length));
//      float sY_ = old_sY + j * (y2 - old_sY) / (MAX_STEPS_PER_SEGMENT * (segmentRemainder / length));
//
//      int dX = xyToAlphaDigit(sX_, sY_);
//      int dY = yToBetaDigit(sY_);
//
//      if (old_dX != dX || old_dY != dY) {
//        fprintf(fp, "[%.5f,%.5f],", digitToAlpha(dX), digitToBeta(dY));
//        coords++;
//      } else {
//        doubleValues++;
//      }
//
//      old_dX = dX;
//      old_dY = dY;
//    }
//  }
//
//  fclose(fp);
//
//  printf("coords:%d doubleValues: %d", coords, doubleValues);
  return 0;
}

//        fprintf(fp, "Position: %.3f,%.3f\n", sX_, sY_);
//        fprintf(fp, "Digital:  %d,%d\n", dX, dY);
//        fprintf(fp, "Winkel:   %.3f,%.3f\n", digitToAlpha(dX), digitToBeta(dY));
//        fprintf(fp, "\n");
