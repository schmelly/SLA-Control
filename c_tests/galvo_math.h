/*
 * galvo_math.h
 *
 *  Created on: 06.04.2015
 *      Author: schmelly
 */

#ifndef GALVO_MATH_H_
#define GALVO_MATH_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * function declarations
 */
int32_t xyToAlphaDigit(int32_t x, int32_t y);
int32_t yToBetaDigit(int32_t y);

float digitToAlpha(int x);
float digitToBeta(int y);

//int xyToAlphaDigit2(float x, float y);
//int yToBetaDigit2(float y);
//float xyToAlpha2(float x, float y);
//float yToBeta2(float y);
//int alphaToDigit2(float alpha);
//int betaToDigit2(float beta);
//int angleToDigit2(float angle, float angle_max);
//float arctan_pade2(float x);

#ifdef __cplusplus
}
#endif

#endif /* GALVO_MATH_H_ */
