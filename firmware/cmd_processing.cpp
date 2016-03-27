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
void linearMoveHelper(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t stepsPerSegment);
void printLoadedParams();
//void axisCompensation(struct gCode* code);
void homeMin(struct gCode* code);
void homeMax(struct gCode* code);
void setAxisDimensions(struct gCode* code);
void setLineSegmentation(struct gCode* code);
void setTiming(struct gCode* code);
void setDipping(struct gCode* code);

int laser_intensity = 0;
double posZ = 0;
int stepsZ = 0;

int32_t oldX = 0, oldY = 0;
int oldXDigit = 0, oldYDigit = 0;

void setupCommandProcessing() {
  laser_intensity = 0;
  oldX = 0;
  oldY = 0;
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
  case M0:
    sleepM0(code);
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
  case M14:
    setLineSegmentation(code);
    Serial.println("ok");
    break;
  case M15:
    setTiming(code);
    Serial.println("ok");
    break;
  case M16:
    setDipping(code);
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

  int32_t x0 = oldX;
  int32_t y0 = oldY;
  int32_t x1 = float2Fixed(xy[0]);
  int32_t y1 = float2Fixed(xy[1]);

  int32_t dX = sub32(x1, x0); // (x1 - x0)
  int32_t dY = sub32(y1, y0); // (y1 - y0)
  int32_t tmp1 = sq32(dX); // (x1 - x0)^2
  int32_t tmp2 = sq32(dY); // (y1 - y0)^2

  int32_t length = sqrt32(add32(tmp1, tmp2));

  int32_t segmentRemainder = length % config.segmenthLength;

  int32_t tmp3 = div32(segmentRemainder, length); // (segmentRemainder / length)
  int32_t tmp4 = mul32(dX, tmp3); // (x1 - x0) * (segmentRemainder / length)
  int32_t tmp5 = mul32(dY, tmp3); // (y1 - y0) * (segmentRemainder / length)

  //float x1_ = x1 - (x1 - x0) * (segmentRemainder / length);
  int32_t x1_ = sub32(x1, tmp4);
  //float y1_ = y1 - (y1 - y0) * (segmentRemainder / length);
  int32_t y1_ = sub32(y1, tmp5);

  int32_t numSegments = (int32_t) fixed2Float(div32(length, config.segmenthLength));

  //int numSegmentsCoords = (numSegments) * 2;
  int32_t numSegmentsCoords = numSegments * 2;
  int32_t segmentCoords[numSegmentsCoords];

  int32_t tmp6 = sub32(x1_, x0); // (x1_ - x0)
  int32_t tmp7 = sub32(y1_, y0); // (y1_ - y0)

  int32_t i_x = div32(tmp6, float2Fixed(numSegments)); // (x1_ - x0) / numSegments
  int32_t i_y = div32(tmp7, float2Fixed(numSegments)); // (y1_ - y0) / numSegments

  int32_t i;
  for (i = 0; i < numSegmentsCoords; i += 2) {

    x0 = add32(x0, i_x);
    y0 = add32(y0, i_y);

    segmentCoords[i] = x0;
    segmentCoords[i + 1] = y0;
  }

  //first part: process each segment
  oldXDigit = xyToAlphaDigit(oldX, oldY);
  oldYDigit = yToBetaDigit(oldY);

  changeLaserValue(laser_intensity);

  for (i = 0; i < numSegmentsCoords; i += 2) {

    int32_t sX = segmentCoords[i];
    int32_t sY = segmentCoords[i + 1];

    linearMoveHelper(oldX, oldY, sX, sY, config.maxStepsPerSegment);

    oldX = sX;
    oldY = sY;
  }

  //2. step last segment to endpoint
  if (segmentRemainder != 0) {

    int32_t stepsPerSegment = mul32(segmentRemainder, div32(config.maxStepsPerSegment, config.segmenthLength));
    linearMoveHelper(oldX, oldY, x1, y1, stepsPerSegment);
  }

  changeLaserValue(0);
  delayMicroseconds(100);

  oldX = x1;
  oldY = y1;
}

void handleNonPrintingMove(struct gCode* code) {

  int32_t x = float2Fixed(code->xCoord);
  int32_t y = float2Fixed(code->yCoord);

  int32_t xDigit = xyToAlphaDigit(x, y);
  int32_t yDigit = yToBetaDigit(y);

  changeMotorValues(xDigit, yDigit);
  delayMicroseconds(100);

  oldX = x;
  oldY = y;
  oldXDigit = xDigit;
  oldYDigit = yDigit;
}

void linearMoveHelper(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t stepsPerSegment) {

  int32_t i;

  uint32_t calcStart;
  int32_t waitTime;

  int32_t dx = sub32(x1, x0); // (x1 - x0)
  int32_t dy = sub32(y1, y0); // (y1 - y0)

  int32_t i_x = div32(dx, stepsPerSegment); // (x1 - x0) / stepsPerSegment
  int32_t i_y = div32(dy, stepsPerSegment); // (y1 - y0) / stepsPerSegment

  calcStart = micros();

  //uint32_t totalWaitTime = 0;
  //uint32_t start = calcStart;

  for (i = 0; i < fixed2Float(stepsPerSegment); i++) {

    int32_t xDigit = xyToAlphaDigit(x0, y0);
    int32_t yDigit = yToBetaDigit(y0);

    //serialPrintln("%.5f\t%.5f\t%d", fixed2Float(x0), fixed2Float(y0), (oldXDigit != xDigit || oldYDigit != yDigit) ? 1 : 0);

    if (oldXDigit != xDigit || oldYDigit != yDigit) {

      changeMotorValues(xDigit, yDigit);

      //calculate the exposure time for a single spot
      waitTime = (int32_t)(config.exposureTime - (micros() - calcStart));

      if (waitTime > 0) {
        //totalWaitTime += waitTime;
        delayMicroseconds(waitTime);
      }

      calcStart = micros();
    }

    x0 = add32(x0, i_x);
    y0 = add32(y0, i_y);

    oldXDigit = xDigit;
    oldYDigit = yDigit;
  }

  //uint32_t total = calcStart - start;
  //serialPrintln("Iterations: %ld Total: %ldus Wait: %ldus Utilization: %.5f%%", i, total, totalWaitTime,
  //    100.0f - (float) totalWaitTime / (float) total * 100.0f);
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

    // dip
    steps = (int) (STEPS_PER_MM_Z * config.dipDepth);
    Stepper.move(steps);
    //posZ += DIP_DEPTH;
    posZ += config.dipDepth;
    stepsZ += steps;

    delay(config.dipTime);

    // un-dip minus targetZ
    steps = (int) (-STEPS_PER_MM_Z * (config.dipDepth - targetZ));
    Stepper.move(steps);
    posZ += -(config.dipDepth - deltaZ);
    stepsZ += steps;

    //let the resin settle;
    delay(config.settleTime);

  } else {
    steps = (int) (STEPS_PER_MM_Z * (zCoord - posZ));
    Stepper.move(steps);
    posZ += (zCoord - posZ);
    stepsZ += steps;
  }
  Stepper.sleepON();
}

void handleHome(struct gCode* code) {

  Stepper.sleepOFF();
  int steps = (int) -stepsZ;
  Stepper.move(steps);
  Stepper.sleepON();
  posZ = 0;
  stepsZ = 0;
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

void setLineSegmentation(struct gCode* code) {

  if (code->xCoord != -FLT_MAX) {
    config.segmenthLength = float2Fixed(code->xCoord);
  }

  if (code->sValue != INT_MIN) {
    config.maxStepsPerSegment = float2Fixed(code->sValue);
  }
}

void setTiming(struct gCode* code) {

  if (code->sValue != INT_MIN) {
    config.settleTime = code->sValue;
  }

  if (code->pValue != INT_MIN) {
    config.exposureTime = code->pValue;
  }
}

void setDipping(struct gCode* code) {

  if (code->zCoord != -FLT_MAX) {
    config.dipDepth = code->zCoord;
  }
  if (code->sValue != INT_MIN) {
    config.dipTime = code->sValue;
  }
}
