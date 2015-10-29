/*
 * config.h
 *
 *  Created on: 06.04.2015
 *      Author: schmelly
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "math.h"
#include "fixedpoint_math.h"

#define SEGMENT_LENGTH float2Fixed(50.0f)
#define MAX_STEPS_PER_SEGMENT float2Fixed(200)

#define DISTANCE_XY_PLANE 200.0f
#define DISTANCE_AB_GALVOS 50.0f

#define ALPHA_MAX 0.359f
#define BETA_MAX 0.359f

#define BITS_DAC 12

#endif /* CONFIG_H_ */
