/*
 * state_handling.cpp
 *
 *  Created on: 07.06.2015
 *      Author: schmelly
 */

//#include "Arduino.h"
#include "state_handling.h"
#include "cmd_handling.h"
#include "cmd_processing.h"

enum states {
  INITIAL, IDLE, POLLING_CMD, PROCESSING_CMD, RENDERING_POINTCLOUD
};

enum states current_state;

void atInitial();
void atIdle();
void atPollingCmd();
void atProcessingCmd();

bool renderingPointCloud = false;

void setupStateHandling() {

  current_state = INITIAL;
  sendInitComplete();
}

void performStep() {

  switch (current_state) {
  case INITIAL:
    atInitial();
    break;
  case IDLE:
    atIdle();
    break;
  case POLLING_CMD:
    atPollingCmd();
    break;
  case PROCESSING_CMD:
    atProcessingCmd();
    break;
  default:
    current_state = INITIAL;
    break;
  }
}

void atInitial() {
  //Serial.println("atInitial");
  current_state = IDLE;
}

void atIdle() {
  //Serial.println("atIdle");
  current_state = POLLING_CMD;

  if (renderingPointCloud) {
    renderPointCloud();
  }
}

void atPollingCmd() {
  //Serial.println("atPollingCmd");
  if (nextCommand()) {
    current_state = PROCESSING_CMD;
  } else {
    current_state = IDLE;
  }
}

void atProcessingCmd() {
  //Serial.println("atProcessingCmd");
  current_state = IDLE;
  gCode* code = getParsedGCode();
  processCommand(code);

  if (code->code == M710) {
    if (code->sValue == 1) {
      renderingPointCloud = true;
    } else {
      renderingPointCloud = false;
    }
  }
}
