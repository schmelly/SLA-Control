/*
 * cmd_processing.cpp
 *
 *  Created on: 07.06.2015
 *      Author: schmelly
 */

#include "Arduino.h"
#include "cmd_processing.h"
#include "spi.h"
#include "config.h"
#include "galvo_math.h"
#include "stepper.h"

void handleLinearMove();
void handleZMove();
void handleLaser();

float oldX = 0.0f, oldY = 0.0f;

void processCommand(enum commands cmd) {
  switch (cmd) {
  case LINEAR_MOVE:
    handleLinearMove();
    break;
  case Z_MOVE:
    handleZMove();
    break;
  case LASER:
    handleLaser();
    break;
  default:
    break;
  }
}

void handleLinearMove() {

  float* xy = getXYCoords();

  // #########################################

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

  unsigned long start, stop;
  start = millis();

  changeLaserValue(196);

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
        changeMotorValues(dX, dY);
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
        changeMotorValues(dX, dY);
      } else {
        doubleValues++;
      }

      old_dX = dX;
      old_dY = dY;
    }
  }

  oldX = x2;
  oldY = y2;

  changeLaserValue(0);

  stop = millis();

  char msg[50];
  memset(msg, 0, sizeof(msg));
  sprintf(msg, "PPS: %.5f", (float) (stop - start) / 1000.0f);
  Serial.println(msg);

  // #########################################

  //changeMotorValues(xy[0], xy[1]);
  //char msg[50];
  //memset(msg, 0, sizeof(msg));
  //sprintf(msg, "Received move command from %f to %f", xy[0], xy[1]);
  //Serial.println(msg);
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
