/*
 * stepper.cpp
 *
 *  Created on: 28.10.2015
 *      Author: schmelly
 */

#include "stepper.h"
#include "AH_Pololu.h"
#include "config.h"

AH_Pololu Stepper( //
    STEPS_PER_TURN, //
    STEPPER_DIR, //
    STEPPER_STEP, //
    STEPPER_MS1, //
    STEPPER_MS2, //
    STEPPER_MS3, //
    STEPPER_SLEEP, //
    STEPPER_ENABLE, //
    STEPPER_RESET);

void setupStepper() {

  Stepper.resetDriver();
  Stepper.enableDriver();
  Stepper.setMicrostepping(QUARTER_STEPMODE);
  Stepper.setSpeedRPM(60);
}
