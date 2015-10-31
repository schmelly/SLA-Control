/*
 * stepper.h
 *
 *  Created on: 28.10.2015
 *      Author: schmelly
 */

#ifndef STEPPER_H_
#define STEPPER_H_

#include "config.h"
#include "AH_Pololu.h"

#ifdef __cplusplus
extern "C" {
#endif

extern AH_Pololu Stepper;
void setupStepper();

#ifdef __cplusplus
}
#endif

#endif /* STEPPER_H_ */
