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

#define serialPrintln(...)  do{ char msg[100];  memset(msg, '\0', sizeof(char) * 100); sprintf(msg, __VA_ARGS__);  Serial.println(msg); } while(0);

#define SEGMENT_LENGTH float2Fixed(5.f)
#define MAX_STEPS_PER_SEGMENT float2Fixed(50.f)

//#define DISTANCE_XY_PLANE 240.f
#define DISTANCE_XY_PLANE 366.5f
#define DISTANCE_AB_GALVOS 9.f

#define X_MAX 61.3f
#define X_MIN -57.0f
#define Y_MAX 91.4f
#define Y_MIN -100.5f

//#define INVERT_X
#define INVERT_Y

#define BITS_DAC 12
#define DAC_MAX ((1 << BITS_DAC) - 1)
#define DAC_ZERO ((1 << BITS_DAC) / 2 - 1)

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

// volume of the two platform profiles per 10mm, used to correct dip depth per layer
#define VOLUME_PROFILES (2*1839.f)

// tank area
#define TANK_Y 175.f
#define TANK_X 140.f
#define TANK_AREA (TANK_X*TANK_Y)

// 3s
#define DIP_TIME 3000
// 0.15ms
#define EXPOSURE_TIME 150
// 4.5s
#define SETTLE_TIME 4500

#ifdef __cplusplus
extern "C" {
#endif

struct Configuration {
  int configInitialized;
  //segment related data
  int32_t segmenthLength;
  int32_t maxStepsPerSegment;
  //projection dimensions
  float distanceXYPlane;
  float xMax;
  float xMin;
  float yMax;
  float yMin;
  float alphaMin;
  float alphaMax;
  float betaMin;
  float betaMax;
  //printing related data
  float dipDepth;
  int32_t dipTime;
  int32_t exposureTime;
  int32_t settleTime;
};

extern Configuration config;

void setupConfig();
void storeConfiguration();
void loadConfiguration();
void loadDefaultValues();
void storeDefaultValues();
void printLoadedParams();

#ifdef __cplusplus
}
#endif

#endif /* CONFIG_H_ */
