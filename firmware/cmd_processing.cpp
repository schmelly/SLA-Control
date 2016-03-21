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

#include "config.h"
#include "fixedpoint_math.h"
#include "galvo_math.h"
#include "spi.h"
#include "stepper.h"

#define XD_MIN 0
#define XD_MAX 4095
#define YD_MIN 0
#define YD_MAX 4095

void handleMove(struct gCode* code);
void handlePrintingMove(struct gCode* code);
void handleNonPrintingMove(struct gCode* code);
void handleZMove(struct gCode* code);
void handleHome(struct gCode* code);
void handleSetPosition(struct gCode* code);
void laserIntensity(struct gCode* code);
void sleepM0(struct gCode* code);
void linearMoveHelper(int32_t sX, int32_t sY, int32_t stepsPerSegment);
void printLoadedParams();
//void axisCompensation(struct gCode* code);
void homeMin(struct gCode* code);
void homeMax(struct gCode* code);
void setAxisDimensions(struct gCode* code);

int laser_intensity = 0;
float oldX = 0.0f, oldY = 0.0f;

double posZ = 0;
int stepsZ = 0;

int32_t old_sX;
int32_t old_sY;
int32_t old_dX;
int32_t old_dY;

void setupCommandProcessing() {
  laser_intensity = 0;
  oldX = 0.0f;
  oldY = 0.0f;
  posZ = 0;
  stepsZ = 0;
  axisDimensionsChanged();
}

void processCommand(struct gCode* code) {

  //printCode(code);

  switch (code->code) {
  case G0:
  case G1:
    handleMove(code);
    handleZMove(code);
    Serial.println("ok");
    break;
  case G28:
    handleHome(code);
    Serial.println("ok");
    break;
  case G92:
    handleSetPosition(code);
    Serial.println("ok");
    break;
  case G161:
    homeMin(code);
    Serial.println("ok");
    break;
  case G162:
    homeMax(code);
    Serial.println("ok");
    break;
  case M12:
    laserIntensity(code);
    Serial.println("ok");
    break;
  case M13:
    if (code->sValue > 0) {
      changeLaserValue(laser_intensity);
    } else {
      changeLaserValue(0);
    }
    Serial.println("ok");
    break;
  case M0:
    sleepM0(code);
    Serial.println("ok");
    break;
  case M105:
    Serial.println("ok T:0 B:0");
    break;
  case M208:
    setAxisDimensions(code);
    Serial.println("ok");
    break;
  case M500:
    storeConfiguration();
    Serial.println("ok");
    break;
  case M501:
    loadConfiguration();
    Serial.println("ok");
    break;
  case M502:
    storeDefaultValues();
    Serial.println("ok");
    break;
  case M503:
    printLoadedParams();
    Serial.println("ok");
    break;
//  case M556:
//    axisCompensation(code);
//    Serial.println("ok");
//    break;
  case M110:
  case UNKNOWN:
    Serial.println("ok");
    break;
  }
}

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

//  dots = 0;
//  uint32_t start = micros();

  int32_t tmp1 = sub32(x1, x2); // (x1 - x2)
  int32_t tmp2 = sub32(y1, y2); // (y1 - y2)
  int32_t tmp3 = sq32(tmp1); // (x1 - x2)^2
  int32_t tmp4 = sq32(tmp2); // (y1 - y2)^2

  //float length = sqrt((x1 - x2)^2 + (y1 - y2)^2));
  int32_t length = sqrt32(add32(tmp3, tmp4));

  //float segmentRemainder = fmod(length, SEGMENT_LENGTH);
  int32_t segmentRemainder = length % config.segmenthLength;

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
  int32_t numSegments = div32(length, config.segmenthLength);

  //int numSegmentsCoords = (numSegments) * 2;
  int32_t numSegmentsCoords = (int32_t) fixed2Float(numSegments) * 2;
  int32_t segmentCoords[numSegmentsCoords];

  int32_t tmp10 = sub32(x2_, x1); // (x2_ - x1)
  int32_t tmp11 = sub32(y2_, y1); // (y2_ - y1)

  int32_t i, j = 0;
  for (i = 0; i < numSegmentsCoords; i += 2) {

    j++;
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

    linearMoveHelper(sX, sY, config.maxStepsPerSegment);

    old_sX = sX;
    old_sY = sY;
  }

  //2. step last segment to endpoint
  if (segmentRemainder != 0) {

    int32_t stepsPerSegment = mul32(config.maxStepsPerSegment, div32(segmentRemainder, length));
    linearMoveHelper(x2, y2, stepsPerSegment);
  }

  changeLaserValue(0);
  delayMicroseconds(100);

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
  delayMicroseconds(100);

  oldX = x;
  oldY = y;
}

void linearMoveHelper(int32_t sX, int32_t sY, int32_t stepsPerSegment) {

  int32_t i;

  uint32_t calcStart;
  int32_t waitTime;

  int32_t tmp1 = sub32(sX, old_sX); // (sX - old_sX)
  int32_t tmp2 = sub32(sY, old_sY); // (sY - old_sY)

  int32_t tmp3 = div32(tmp1, stepsPerSegment); // (sX - old_sX) / stepsPerSegment
  int32_t tmp4 = div32(tmp2, stepsPerSegment); // (sY - old_sY) / stepsPerSegment

  calcStart = micros();

  for (i = 0; i < fixed2Float(stepsPerSegment); i++) {

    int32_t tmp5 = mul32(float2Fixed(i), tmp3); // i * (sX - old_sX) / stepsPerSegment
    int32_t tmp6 = mul32(float2Fixed(i), tmp4); // i* (sY - old_sY) / stepsPerSegment

    //float sX_ = old_sX + i * (sX - old_sX) / stepsPerSegment;
    int32_t sX_ = add32(old_sX, tmp5);
    //float sY_ = old_sY + i * (sY - old_sY) / stepsPerSegment;
    int32_t sY_ = add32(old_sY, tmp6);

    int32_t dX = xyToAlphaDigit(sX_, sY_);
    int32_t dY = yToBetaDigit(sY_);

    if (old_dX != dX || old_dY != dY) {
      changeMotorValues(dX, dY);
      //printf("%d|%d\n", dX, dY);
      //dots++;

      //calculate the exposure time for a single spot
      //waitTime = (int32_t)(EXPOSURE_TIME - (micros() - calcStart));
      waitTime = (int32_t)(config.exposureTime - (micros() - calcStart));

      if (waitTime > 0) {
        delayMicroseconds(waitTime);
      }

//      char msg[100];
//      memset(msg, '\0', sizeof(char) * 100);
//      sprintf(msg, "old_dX: %ld old_dY: %ld dX: %ld dY: %ld waitTime: %ld", old_dX, old_dY, dX, dY, waitTime);
//      Serial.println(msg);

      calcStart = micros();
    }

    old_dX = dX;
    old_dY = dY;
  }
}

void handleZMove(struct gCode* code) {

  if (code->zCoord == -FLT_MAX || code->zCoord == posZ) {
    return;
  }

  float zCoord = code->zCoord;
  int steps;

  Stepper.sleepOFF();
  if (code->gCodeDigit == 1) {

    // calculate relative distance to move
    double deltaZ = zCoord - posZ;
    // calculate the volume we add due to the 2 profiles
    double volumeProfiles = VOLUME_PROFILES * deltaZ;
    // calculate the layer volume
    double layerVolume = TANK_AREA * deltaZ;
    // calculate target z
    double targetZ = (layerVolume - volumeProfiles) / TANK_AREA;

//    memset(msg, '\0', sizeof(char) * 100);
//    sprintf(msg, "vP: %f | lV: %f | tZ: %f", volumeProfiles, layerVolume, targetZ);
//    Serial.println(msg);

    // dip
    steps = (int) (STEPS_PER_MM_Z * config.dipDepth);
    Stepper.move(steps);
    //posZ += DIP_DEPTH;
    posZ += config.dipDepth;
    stepsZ += steps;

    delay(2000);

//    memset(msg, '\0', sizeof(char) * 100);
//    sprintf(msg, "deltaZ: %f | posZ: %f | stepsZ: %d | dip: %d steps", deltaZ, posZ, stepsZ, steps);
//    Serial.println(msg);

    // un-dip minus targetZ
    steps = (int) (-STEPS_PER_MM_Z * (config.dipDepth - targetZ));
    Stepper.move(steps);
    posZ += -(config.dipDepth - deltaZ);
    stepsZ += steps;

//    memset(msg, '\0', sizeof(char) * 100);
//    sprintf(msg, "deltaZ: %f | posZ: %f | stepsZ: %d | un-dip: %d steps", deltaZ, posZ, stepsZ, steps);
//    Serial.println(msg);

    //let the resin settle;
    //delay (SETTLE_TIME);
    delay(config.settleTime);

  } else {
    steps = (int) (STEPS_PER_MM_Z * (zCoord - posZ));
    Stepper.move(steps);
    posZ += (zCoord - posZ);
    stepsZ += steps;
  }
  Stepper.sleepON();

//  memset(msg, '\0', sizeof(char) * 100);
//  sprintf(msg, "posZ: %f | stepsZ: %d | %d steps", posZ, stepsZ, steps);
//  Serial.println(msg);
}

void handleHome(struct gCode* code) {

  Stepper.sleepOFF();
  int steps = (int) -stepsZ;
  Stepper.move(steps);
  Stepper.sleepON();
  posZ = 0;
  stepsZ = 0;

//  char msg[100];
//  memset(msg, '\0', sizeof(char) * 100);
//  sprintf(msg, "posZ: %f | stepsZ: %d | home %d steps", posZ, stepsZ, steps);
//  Serial.println(msg);
}

void sleepM0(struct gCode* code) {

  if (code->pValue != INT_MIN) {
    delay(code->pValue);
  }
}

void handleSetPosition(struct gCode* code) {

  if (code->xCoord != -FLT_MAX) {
    oldX = 0.f;
  }

  if (code->yCoord != -FLT_MAX) {
    oldY = 0.f;
  }

  if (code->zCoord != -FLT_MAX) {
    posZ = 0.0;
    stepsZ = 0;
  }
}

void laserIntensity(struct gCode* code) {

  if (code->sValue == INT_MIN) {
    return;
  }

  laser_intensity = code->sValue;
  //int value = code->laserIntensity;
  //changeLaserValue(value);

//  char msg[100];
//  memset(msg, '\0', sizeof(char) * 100);
//  sprintf(msg, "laser intensity: %d", laser_intensity);
//  Serial.println(msg);
}

//void axisCompensation(struct gCode* code) {
//
//  if (code->xCoord != -FLT_MAX) {
//    code->sValue == 1 ? config.xMinOffset = (code->xCoord) - config.xMin : config.xMaxOffset = (code->xCoord) - config.xMax;
//  }
//
//  if (code->yCoord != -FLT_MAX) {
//    code->sValue == 1 ? config.yMinOffset = (code->yCoord) - config.yMin : config.yMaxOffset = (code->yCoord) - config.yMax;
//  }
//}

void homeMin(struct gCode* code) {

  if (code->xCoord != -FLT_MAX && code->yCoord != -FLT_MAX) {
    changeMotorValues(XD_MIN, YD_MIN);
  } else if (code->xCoord != -FLT_MAX) {
    changeMotorValues(XD_MIN, DAC_ZERO);
  } else if (code->yCoord != -FLT_MAX) {
    changeMotorValues(DAC_ZERO, YD_MIN);
  }
}

void homeMax(struct gCode* code) {

  if (code->xCoord != -FLT_MAX && code->yCoord != -FLT_MAX) {
    changeMotorValues(XD_MAX, YD_MAX);
  } else if (code->xCoord != -FLT_MAX) {
    changeMotorValues(XD_MAX, DAC_ZERO);
  } else if (code->yCoord != -FLT_MAX) {
    changeMotorValues(DAC_ZERO, YD_MAX);
  }
}

void setAxisDimensions(struct gCode* code) {

  if (code->xCoord != -FLT_MAX) {
    code->sValue == 1 ? config.xMin = code->xCoord : config.xMax = code->xCoord;
  }

  if (code->yCoord != -FLT_MAX) {
    code->sValue == 1 ? config.yMin = code->yCoord : config.yMax = code->yCoord;
  }

  if (code->zCoord != -FLT_MAX) {
    config.distanceXYPlane = code->zCoord;
  }

  axisDimensionsChanged();
}
