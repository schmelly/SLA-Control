/*
 * config.h
 *
 *  Created on: 06.04.2015
 *      Author: schmelly
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "math.h"

#define SEGMENT_LENGTH float2Fixed(5.f)
#define MAX_STEPS_PER_SEGMENT float2Fixed(66.f)

#define DISTANCE_XY_PLANE 240.f
#define DISTANCE_AB_GALVOS 9.f

#define X_MAX 40.f
#define X_MIN -40.f

#define Y_MAX 65.f
#define Y_MIN -65.f

#define ALPHA_MAX (float)tan(X_MAX/DISTANCE_XY_PLANE)
#define BETA_MAX (float)tan(Y_MAX/DISTANCE_XY_PLANE)

//#define INVERT_X
#define INVERT_Y

#define BITS_DAC 12

//pin definitions
#define STEPPER_DIR 31
#define STEPPER_STEP 33
#define STEPPER_SLEEP 35
#define STEPPER_RESET 37
#define STEPPER_MS3 39
#define STEPPER_MS2 41
#define STEPPER_MS1 43
#define STEPPER_ENABLE 45

#define Z_PITCH 1.5
#define STEPS_PER_TURN 200
#define STEPPING 4
#define STEPS_PER_MM_Z (STEPPING*STEPS_PER_TURN/Z_PITCH)

// how much is the platform dipped during z-moves
#define DIP_DEPTH 4.f

// volume of the two platform profiles per 10mm, use to correct dip depth per layer
#define VOLUME_PROFILES (2*1839.f)

// tank area
#define TANK_Y 175.f
#define TANK_X 140.f
#define TANK_AREA (TANK_X*TANK_Y)

// 0.25 ms
#define EXPOSURE_TIME 150
// 20s
#define SETTLE_TIME 5000

#endif /* CONFIG_H_ */
