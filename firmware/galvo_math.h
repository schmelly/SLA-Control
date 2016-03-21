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

float digitToAlpha(int32_t x);
float digitToBeta(int32_t y);

void axisDimensionsChanged();

#ifdef __cplusplus
}
#endif

#endif /* GALVO_MATH_H_ */
