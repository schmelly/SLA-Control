/*
 * galvo_math.c
 *
 *  Created on: 06.04.2015
 *      Author: schmelly
 */

#include <stdio.h>

#include "math.h"
#include "config.h"
#include "galvo_math.h"
#include "fixedpoint_math.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ALPHA_MAX_F float2Fixed(ALPHA_MAX)
#define BETA_MAX_F float2Fixed(BETA_MAX)
#define DAC_MAX_F float2Fixed((float) (1 << BITS_DAC))
#define C05 float2Fixed(1.f / 2.f)
#define C1 float2Fixed(1.f)
#define C2 float2Fixed(2.f)
#define DISTANCE_XY_PLANE_F float2Fixed(DISTANCE_XY_PLANE)
#define DISTANCE_AB_GALVOS_F float2Fixed(DISTANCE_AB_GALVOS)

const int dac_max = 1 << BITS_DAC;

/**
 * function declarations
 */
int32_t xyToAlpha(int32_t x, int32_t y);
int32_t yToBeta(int32_t y);

int32_t alphaToDigit(int32_t alpha);
int32_t betaToDigit(int32_t beta);
int32_t angleToDigit(int32_t angle, int32_t angle_max);

/**
 * function definitions
 * TODO fixed point, optimizations
 */

int32_t xyToAlphaDigit(float x, float y) {

  //float alpha = xyToAlpha(x, y);

  int32_t result;

  int32_t x_f = float2Fixed(x);
  int32_t y_f = float2Fixed(y);

  int32_t alpha_f = xyToAlpha(x_f, y_f);

  result = alphaToDigit(alpha_f);
  result = (int32_t) fixed2Float(result);

  return result;
}

int yToBetaDigit(float y) {
  //float beta = yToBeta(y);

  int32_t result;

  int32_t y_f = float2Fixed(y);

  int32_t beta_f = yToBeta(y_f);

  result = betaToDigit(beta_f);
  result = (int32_t) fixed2Float(result);

  return result;
}

float digitToAlpha(int x) {

  //2.f * ALPHA_MAX * ((float) x / (float) dac_max - 1.f / 2.f)

  float result;

  int32_t x_f = float2Fixed(x);

  int32_t tmp1 = q_div(x_f, DAC_MAX_F);
  int32_t tmp2 = q_sub(tmp1, C05);
  int32_t tmp3 = q_mul(q_mul(C2, ALPHA_MAX_F), tmp2);

  result = fixed2Float(tmp3);

  return result;
}

float digitToBeta(int y) {

  //2.f * BETA_MAX * ((float) y / (float) dac_max - 1.f / 2.f)

  float result;

  int32_t y_f = float2Fixed(y);

  int32_t tmp1 = q_div(y_f, DAC_MAX_F);
  int32_t tmp2 = q_sub(tmp1, C05);
  int32_t tmp3 = q_mul(q_mul(C2, ALPHA_MAX_F), tmp2);

  result = fixed2Float(tmp3);

  return result;
}

int32_t xyToAlpha(int32_t x, int32_t y) {

  //arctan_pade(x / (DISTANCE_AB_GALVOS + sqrt(DISTANCE_XY_PLANE * DISTANCE_XY_PLANE + y * y)));

  int32_t result;

  int32_t tmp1 = q_mul(DISTANCE_XY_PLANE_F, DISTANCE_XY_PLANE_F);
  int32_t tmp2 = q_mul(y, y);
  int32_t tmp3 = q_add(tmp1, tmp2);
  int32_t tmp4 = q_add(DISTANCE_AB_GALVOS_F, sqrt_f(tmp3));

  result = arctan_pade(q_div(x, tmp4));

  return result;
}

int32_t yToBeta(int32_t y) {

  int32_t result;

  result = arctan_pade(q_div(y, DISTANCE_XY_PLANE_F));

  return result;
}

int32_t alphaToDigit(int32_t alpha) {
  return angleToDigit(alpha, ALPHA_MAX_F);
}

int32_t betaToDigit(int32_t beta) {
  return angleToDigit(beta, BETA_MAX_F);
}

int32_t angleToDigit(int32_t angle, int32_t angle_max) {

  //dac_max * (angle + angle_max) / (2 * angle_max);

  int32_t result;

  int32_t tmp1 = q_add(angle, angle_max);
  int32_t tmp2 = q_mul(C2, angle_max);

  result = q_div(q_mul(DAC_MAX_F, tmp1), tmp2);

  return result;
}

int xyToAlphaDigit2(float x, float y) {
  float alpha = xyToAlpha2(x, y);
  return alphaToDigit2(alpha);
}

int yToBetaDigit2(float y) {
  float beta = yToBeta2(y);
  return betaToDigit2(beta);
}

float xyToAlpha2(float x, float y) {

  return atan(x / (DISTANCE_AB_GALVOS + sqrt(DISTANCE_XY_PLANE * DISTANCE_XY_PLANE + y * y)));
}

float yToBeta2(float y) {

  return atan(y / DISTANCE_XY_PLANE);
}

int alphaToDigit2(float alpha) {
  return angleToDigit2(alpha, ALPHA_MAX);
}

int betaToDigit2(float beta) {
  return angleToDigit2(beta, BETA_MAX);
}

int angleToDigit2(float angle, float angle_max) {

  return dac_max * (angle + angle_max) / (2 * angle_max);
}

float arctan_pade2(float x) {
  return 3.f * x / (x * x + 3.f);
}

#ifdef __cplusplus
}
#endif
