/*
 * galvo_math.c
 *
 *  Created on: 06.04.2015
 *      Author: schmelly
 */

#include "math.h"

#include "config.h"
#include "galvo_math.h"

#ifdef __cplusplus
extern "C" {
#endif

const int dac_max = 1 << BITS_DAC;

/**
 * function declarations
 */
float xyToAlpha(float x, float y);
float yToBeta(float y);

int alphaToDigit(float alpha);
int betaToDigit(float beta);
int angleToDigit(float angle, float angle_max);

float arctan_pade(float x);

/**
 * function definitions
 * TODO fixed point, optimizations
 */

int xyToAlphaDigit(float x, float y) {
  float alpha = xyToAlpha(x, y);
  return alphaToDigit(alpha);
}

int yToBetaDigit(float y) {
  float beta = yToBeta(y);
  return betaToDigit(beta);
}

float digitToAlpha(int x) {
  return 2.f * ALPHA_MAX * ((float) x / (float) dac_max - 1.f / 2.f);
}

float digitToBeta(int y) {
  return 2.f * BETA_MAX * ((float) y / (float) dac_max - 1.f / 2.f);
}

float xyToAlpha(float x, float y) {

  return arctan_pade(x / (DISTANCE_AB_GALVOS + sqrt(DISTANCE_XY_PLANE * DISTANCE_XY_PLANE + y * y)));
}

float yToBeta(float y) {

  return arctan_pade(y / DISTANCE_XY_PLANE);
}

int alphaToDigit(float alpha) {
  return angleToDigit(alpha, ALPHA_MAX);
}

int betaToDigit(float beta) {
  return angleToDigit(beta, BETA_MAX);
}

int angleToDigit(float angle, float angle_max) {

  return dac_max * (angle + angle_max) / (2 * angle_max);
}

float arctan_pade(float x) {
  return 3.f * x / (x * x + 3.f);
}

#ifdef __cplusplus
}
#endif
