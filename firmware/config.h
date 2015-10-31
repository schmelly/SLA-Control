/*
 * config.h
 *
 *  Created on: 06.04.2015
 *      Author: schmelly
 */

#ifndef CONFIG_H_
#define CONFIG_H_

//#include "math.h"

#define SEGMENT_LENGTH float2Fixed(5.f)
#define MAX_STEPS_PER_SEGMENT float2Fixed(33.f)

#define DISTANCE_XY_PLANE 200.f
#define DISTANCE_AB_GALVOS 50.f

#define ALPHA_MAX 0.359f
#define BETA_MAX 0.359f

#define BITS_DAC 12

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
#define STEPS_PER_MM_Z STEPPING*STEPS_PER_TURN/Z_PITCH

// 5 ms
#define EXPOSURE_TIME 5*1000

#endif /* CONFIG_H_ */
