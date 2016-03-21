/*
 * config.cpp
 *
 *  Created on: 15.03.2016
 *      Author: schmelly
 */
#include <DueFlashStorage.h>
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

DueFlashStorage dueFlashStorage;
Configuration config;

void setupConfig() {
  loadConfiguration();

  if (config.configInitialized != 1) {
    loadDefaultValues();
    config.configInitialized = 1;
    storeConfiguration();
    serialPrintln("Configuration initialized");
  }
}

void storeConfiguration() {

  byte bytes[sizeof(Configuration)];
  memcpy((void*) bytes, (const void*) &config, sizeof(Configuration));
  dueFlashStorage.write(0, bytes, sizeof(Configuration));
}

void loadConfiguration() {
  byte* bytes = dueFlashStorage.readAddress(0);
  memcpy(&config, bytes, sizeof(Configuration));
}

void loadDefaultValues() {

  config.configInitialized = 0;
  config.segmenthLength = SEGMENT_LENGTH;
  config.maxStepsPerSegment = MAX_STEPS_PER_SEGMENT;
  config.distanceXYPlane = DISTANCE_XY_PLANE;
  config.xMax = X_MAX;
  config.xMin = X_MIN;
  config.yMax = Y_MAX;
  config.yMin = Y_MIN;
  config.alphaMin = (float) abs(atan(config.xMin / config.distanceXYPlane));
  config.alphaMax = (float) abs(atan(config.xMax / config.distanceXYPlane));
  config.betaMin = (float) abs(atan(config.yMin / config.distanceXYPlane));
  config.betaMax = (float) abs(atan(config.yMax / config.distanceXYPlane));
  config.dipDepth = DIP_DEPTH;
  config.exposureTime = EXPOSURE_TIME;
  config.settleTime = SETTLE_TIME;
}

void storeDefaultValues() {

  loadDefaultValues();
  storeConfiguration();
}

void printLoadedParams() {

  serialPrintln("Configuration initialized: %d", config.configInitialized);

  serialPrintln("segmenthLength: %.4f\nmaxStepsPerSegment: %.4f", //
      fixed2Float(config.segmenthLength), fixed2Float(config.maxStepsPerSegment));

  serialPrintln("xyPlane: %.4f\nxMax: %.4f\nxMin: %.4f\nyMax: %.4f\nyMin: %.4f", //
      config.distanceXYPlane, config.xMax, config.xMin, config.yMax, config.yMin);

  serialPrintln("alphaMin: %.4f\nalphaMax: %.4f\nbetaMin: %.4f\nbetaMax: %.4f", //
      config.alphaMin, config.alphaMax, config.betaMin, config.betaMax);

  serialPrintln("dipDepth: %.4f\nexposureTime: %ld\nexposureTime: %ld", //
      config.dipDepth, config.exposureTime, config.settleTime);
}

#ifdef __cplusplus
}
#endif
