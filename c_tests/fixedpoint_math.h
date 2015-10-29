/*
 * fixedpoint_math.h
 *
 *  Created on: 24.10.2015
 *      Author: schmelly
 */

#ifndef FIXEDPOINT_MATH_H_
#define FIXEDPOINT_MATH_H_

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t float2Fixed(float f);
float fixed2Float(int32_t s);

int32_t q_add(int32_t a, int32_t b);
int32_t q_sub(int32_t a, int32_t b);
int32_t q_mul(int32_t a, int32_t b);
int32_t q_div(int32_t a, int32_t b);

int32_t arctan_pade(int32_t x);
uint32_t sqrt_f(uint32_t x);

#ifdef __cplusplus
}
#endif

#endif /* FIXEDPOINT_MATH_H_ */
