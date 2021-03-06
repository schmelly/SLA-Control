/*
 * galvo_math.c
 *
 *  Created on: 06.04.2015
 *      Author: schmelly
 */

#include <stdio.h>
#include <Arduino.h>

#include "config.h"
#include "galvo_math.h"
#include "fixedpoint_math.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DAC_MAX_F float2Fixed(DAC_MAX)
#define C05 float2Fixed(1.f / 2.f)
#define DISTANCE_AB_GALVOS_F float2Fixed(DISTANCE_AB_GALVOS)

int32_t distanceXYPlane = float2Fixed(config.distanceXYPlane);
int32_t xMax = float2Fixed(config.xMax);
int32_t xMin = float2Fixed(config.xMin);
int32_t yMax = float2Fixed(config.yMax);
int32_t yMin = float2Fixed(config.yMin);
int32_t alphaMin = float2Fixed(config.alphaMin);
int32_t alphaMax = float2Fixed(config.alphaMax);
int32_t betaMin = float2Fixed(config.betaMin);
int32_t betaMax = float2Fixed(config.betaMax);

/**
 * function declarations
 */
int32_t xyToAlpha(int32_t x, int32_t y);
int32_t yToBeta(int32_t y);

int32_t alphaToDigit(int32_t alpha, int32_t x);
int32_t betaToDigit(int32_t beta, int32_t y);
int32_t angleToDigit(int32_t angle, int32_t angle_max);

/**
 * function definitions
 */

int32_t xyToAlphaDigit(int32_t x, int32_t y) {

  int32_t result;

  int32_t alpha = xyToAlpha(x, y);

  result = alphaToDigit(alpha, x);
  result = (int32_t) fixed2Float(add32(result, C05));

  return result;
}

int32_t yToBetaDigit(int32_t y) {

  int32_t result;

  int32_t beta = yToBeta(y);

  result = betaToDigit(beta, y);
  result = (int32_t) fixed2Float(add32(result, C05));

  return result;
}

float digitToAlpha(int32_t x) {

  //2.f * ALPHA_MAX * ((float) x / (float) dac_max - 1.f / 2.f)

  float result;

  int32_t x_f = float2Fixed(x);

  int32_t tmp1 = div32(x_f, DAC_MAX_F);
  int32_t tmp2 = sub32(tmp1, C05);
  int32_t tmp3 = mul32(add32(alphaMax, alphaMax), tmp2);

  result = fixed2Float(tmp3);

  return result;
}

float digitToBeta(int32_t y) {

  //2.f * BETA_MAX * ((float) y / (float) dac_max - 1.f / 2.f)

  float result;

  int32_t y_f = float2Fixed(y);

  int32_t tmp1 = div32(y_f, DAC_MAX_F);
  int32_t tmp2 = sub32(tmp1, C05);
  int32_t tmp3 = mul32(add32(alphaMax, alphaMax), tmp2);

  result = fixed2Float(tmp3);

  return result;
}

int32_t xyToAlpha(int32_t x, int32_t y) {

  //arctan_pade(x / (DISTANCE_AB_GALVOS + sqrt(DISTANCE_XY_PLANE * DISTANCE_XY_PLANE + y * y)));

  int32_t result;

  int32_t tmp1 = mul32(distanceXYPlane, distanceXYPlane);
  int32_t tmp2 = mul32(y, y);
  int32_t tmp3 = add32(tmp1, tmp2);
  int32_t tmp4 = add32(DISTANCE_AB_GALVOS_F, sqrt32(tmp3));

  result = arctan_pade32(div32(x, tmp4));

  return result;
}

int32_t yToBeta(int32_t y) {

  int32_t result;

  result = arctan_pade32(div32(y, distanceXYPlane));

  return result;
}

int32_t alphaToDigit(int32_t alpha, int32_t x) {

  if (x >= 0) {
    return angleToDigit(alpha, alphaMax);
  } else {
    return angleToDigit(alpha, alphaMin);
  }
}

int32_t betaToDigit(int32_t beta, int32_t y) {

  if (y >= 0) {
    return angleToDigit(beta, betaMax);
  } else {
    return angleToDigit(beta, betaMin);
  }
}

int32_t angleToDigit(int32_t angle, int32_t angle_max) {

  //dac_max * (angle + angle_max) / (2 * angle_max);

  int32_t result;

  int32_t tmp1 = add32(angle, angle_max);
  int32_t tmp2 = add32(angle_max, angle_max);

  result = div32(mul32(DAC_MAX_F, tmp1), tmp2);

  return result;
}

void axisDimensionsChanged() {

  distanceXYPlane = float2Fixed(config.distanceXYPlane);
  xMax = float2Fixed(config.xMax);
  xMin = float2Fixed(config.xMin);
  yMax = float2Fixed(config.yMax);
  yMin = float2Fixed(config.yMin);

  config.alphaMin = (float) abs(atan(config.xMin / (DISTANCE_AB_GALVOS + config.distanceXYPlane)));
  config.alphaMax = (float) abs(atan(config.xMax / (DISTANCE_AB_GALVOS + config.distanceXYPlane)));
  config.betaMin = (float) abs(atan(config.yMin / config.distanceXYPlane));
  config.betaMax = (float) abs(atan(config.yMax / config.distanceXYPlane));

  alphaMin = float2Fixed(config.alphaMin);
  alphaMax = float2Fixed(config.alphaMax);
  betaMin = float2Fixed(config.betaMin);
  betaMax = float2Fixed(config.betaMax);
}

#ifdef __cplusplus
}
#endif
