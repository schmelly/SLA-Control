/*
 * fixedpoint_math.cpp
 *
 *  Created on: 24.10.2015
 *      Author: schmelly
 */

#include "config.h"
#include "fixedpoint_math.h"
#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t float2Fixed(float f) {

  int32_t result;

  float temp = f * (1 << Qf);
  temp += (temp >= 0) ? 0.5f : -0.5f;

  result = (int32_t) temp;
  return result;
}

float fixed2Float(int32_t s) {

  float result = ((float) s) * 1.0f / (float) (1 << Qf);
  return result;
}

int32_t add32(int32_t a, int32_t b) {

  int32_t result = a + b;
  return result;
}

int32_t sub32(int32_t a, int32_t b) {

  int32_t result = a - b;
  return result;
}

int32_t mul32(int32_t a, int32_t b) {

  int32_t result;

  int64_t temp = (int64_t) a * (int64_t) b;
  temp += K;

  result = (int32_t)(temp >> Qf);
  return result;
}

int32_t div32(int32_t a, int32_t b) {

  int32_t result;
  int64_t temp;

  // pre-multiply by the base (Upscale to Q64 so that the result will be in Q32 format)
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

int32_t sq32(int32_t a) {

  return mul32(a, a);
}

int32_t sqrt32(int32_t x) {

//  int32_t result = float2Fixed(sqrt(fixed2Float(x)));
//  return result;

  uint32_t testDiv = 0;
  uint32_t root = 0;
  uint32_t remHi = 0;
  uint32_t remLo = (int32_t) x;
  int32_t count = 21;  //(15 + b)/2
  do {
    remHi = (remHi << 2) | (remLo >> 30);
    remLo <<= 2;
    root <<= 1;
    testDiv = (root << 1) + 1;
    if (remHi >= testDiv) {
      remHi -= testDiv;
      root += 1;
    }
  } while (count-- != 0);
  return (int32_t) root;
}

int32_t arctan_pade32(int32_t a) {

//  int32_t result = float2Fixed(atan(fixed2Float(a)));
//  return result;

//3.f * x / (x * x + 3.f);
  int32_t tmp1 = mul32(C3, a);
  int32_t tmp2 = add32(mul32(a, a), C3);
  int32_t result = div32(tmp1, tmp2);

  return result;
}

int32_t linearInterpolation(int32_t q1, int32_t q2, int32_t x0, int32_t x1, int32_t x) {

  //e = f(x0) + (f(x1) - f(x0)) / (x1 - x0) * (x - x0)
  int32_t q2q1, x1x0, xx0, returnValue;
  q2q1 = q2 - q1;
  x1x0 = x1 - x0;
  xx0 = x - x0;
  returnValue = q1 + mul32(div32(q2q1, x1x0), xx0);

  return returnValue;
}

int32_t bilinearInterpolation(int32_t q11, int32_t q12, int32_t q21, int32_t q22, int32_t x0, int32_t x1, int32_t y0, int32_t y1, int32_t x, int32_t y) {

  int32_t x1x0, y1y0, x1x, y1y, yy0, xx0, returnValue;
  x1x0 = x1 - x0;
  y1y0 = y1 - y0;
  x1x = x1 - x;
  y1y = y1 - y;
  yy0 = y - y0;
  xx0 = x - x0;
  returnValue = mul32(div32(C1, mul32(x1x0, y1y0)),
      (mul32(q11, mul32(x1x, y1y)) + mul32(q21, mul32(xx0, y1y)) + mul32(q12, mul32(x1x, yy0)) + mul32(q22, mul32(xx0, yy0))));
  return returnValue;
}

#ifdef __cplusplus
}
#endif
