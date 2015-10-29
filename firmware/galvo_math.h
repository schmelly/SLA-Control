/*
 * galvo_math.h
 *
 *  Created on: 06.04.2015
 *      Author: schmelly
 */

#ifndef GALVO_MATH_H_
#define GALVO_MATH_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * function declarations
 */
int xyToAlphaDigit(float x, float y);
int yToBetaDigit(float y);

float digitToAlpha(int x);
float digitToBeta(int y);

#ifdef __cplusplus
}
#endif

#endif /* GALVO_MATH_H_ */
