/*
 * fixedpoint_math.h
 *
 *  Created on: 24.10.2015
 *      Author: schmelly
 */

#ifndef FIXEDPOINT_MATH_H_
#define FIXEDPOINT_MATH_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t float2Fixed(float f);
float fixed2Float(int32_t s);

int32_t add32(int32_t a, int32_t b);
int32_t sub32(int32_t a, int32_t b);
int32_t mul32(int32_t a, int32_t b);
int32_t div32(int32_t a, int32_t b);

int32_t arctan_pade32(int32_t a);
int32_t sq32(int32_t a);
int32_t sqrt32(int32_t a);

#ifdef __cplusplus
}
#endif

#endif /* FIXEDPOINT_MATH_H_ */
