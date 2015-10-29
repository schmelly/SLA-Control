/*
 * fixedpoint_math.cpp
 *
 *  Created on: 24.10.2015
 *      Author: schmelly
 */

#include "fixedpoint_math.h"

#ifdef __cplusplus
extern "C" {
#endif

#define Qm 23
#define Qf 12
#define K (1 << (Qf - 1))

const int32_t c3 = float2Fixed(3.f);

int32_t float2Fixed(float f) {

  int32_t result;

  float temp = f * (1 << Qf);
  temp += (temp >= 0) ? 0.5f : -0.5f;

  result = (int32_t) temp;
  //printf("float2Fixed(%f): %f\n", (double) f, (double) result);

  return (int32_t) result;
}

float fixed2Float(int32_t s) {

  float result = ((float) s) * 1.0f / (float) (1 << Qf);

  return result;
}

int32_t q_add(int32_t a, int32_t b) {

  int32_t result = a + b;

  return result;
}

int32_t q_sub(int32_t a, int32_t b) {

  int32_t result = a - b;

  return result;
}

int32_t q_mul(int32_t a, int32_t b) {

  int32_t result;

  int64_t temp = (int64_t) a * (int64_t) b;
  temp += K;

  result = (int32_t)(temp >> Qf);

  //printf("q_mul(%f,%f): %f\n", (double) a, (double) b, (double) result);

  return result;
}

int32_t q_div(int32_t a, int32_t b) {

  int32_t result;
  int64_t temp;

  // pre-multiply by the base (Upscale to Q16 so that the result will be in Q8 format)
  temp = (int64_t) a << Qf;
  // Rounding: mid values are rounded up (down for negative values).
  if ((temp >= 0 && b >= 0) || (temp < 0 && b < 0)) {
    temp += b / 2;
  } else {
    temp -= b / 2;
  }
  result = (int32_t)(temp / b);

  return result;
}

int32_t q_sq(int32_t a) {

  return q_mul(a, a);
}

int32_t arctan_pade(int32_t x) {

  //3.f * x / (x * x + 3.f);
  int32_t tmp1 = q_mul(c3, x);
  int32_t tmp2 = q_add(q_mul(x, x), c3);
  int32_t result = q_div(tmp1, tmp2);

  return result;
}

uint32_t sqrt_f(uint32_t x) {
  uint32_t root, remHi, remLo, testDiv, count;
  root = 0; /* Clear root */
  remHi = 0; /* Clear high part of partial remainder */
  remLo = x; /* Get argument into low part of partial remainder */
  count = 8; /* Load loop counter */
  do {
    remHi = (remHi << 2) | (remLo >> 30);
    remLo <<= 2; /* get 2 bits of arg */
    root <<= 1; /* Get ready for the next bit in the root */
    testDiv = (root << 1) + 1; /* Test radical */
    if (remHi >= testDiv) {
      remHi -= testDiv;
      root += 1;
    }
  } while (count-- != 0);
  return root;
}

#ifdef __cplusplus
}
#endif
