/*
 * spi.cpp
 *
 *  Created on: 23.07.2015
 *      Author: schmelly
 */
#include "spi.h"
#include "config.h"
#include <SPI.h>

#define CS_MOTORS 4
#define CS_LASER 10

const uint16_t maskA = 0b0011000000000000;
const uint16_t maskB = 0b1011000000000000;

int oldXValue = DAC_ZERO, oldYValue = DAC_ZERO;

void setupSPI() {

  SPI.begin();

  pinMode(CS_MOTORS, OUTPUT);
  digitalWrite(CS_MOTORS, HIGH);
  pinMode(CS_LASER, OUTPUT);
  digitalWrite(CS_LASER, HIGH);

  changeMotorValues(DAC_ZERO, DAC_ZERO);
  changeLaserValue(0);
}

void changeMotorValues(int xValue, int yValue) {

  if (xValue < 0) {
    xValue = 0;
  } else if (xValue > DAC_MAX) {
    xValue = DAC_MAX;
  }

  if (yValue < 0) {
    yValue = 0;
  } else if (yValue > DAC_MAX) {
    yValue = DAC_MAX;
  }

  if (abs(xValue - oldXValue) > 10 || abs(yValue - oldYValue) > 10) {
    int sgnX = ((xValue - oldXValue) > 10) - ((xValue - oldXValue) < -10);
    int sgnY = ((yValue - oldYValue) > 10) - ((yValue - oldYValue) < -10);
    changeMotorValues(xValue - 10 * sgnX, yValue - 10 * sgnY);
  }

  oldXValue = xValue;
  oldYValue = yValue;

#ifdef INVERT_X
  xValue = DAC_MAX-xValue;
#endif
#ifdef INVERT_Y
  yValue = DAC_MAX-yValue;
#endif

  SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
  digitalWrite(CS_MOTORS, LOW);
  SPI.transfer16(((uint16_t) xValue) + maskA);
  digitalWrite(CS_MOTORS, HIGH);
  digitalWrite(CS_MOTORS, LOW);
  SPI.transfer16(((uint16_t) yValue) + maskB);
  digitalWrite(CS_MOTORS, HIGH);
  SPI.endTransaction();
}

void changeLaserValue(int value) {

  SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
  digitalWrite(CS_LASER, LOW);
  SPI.transfer16((uint16_t) value + maskA);
  digitalWrite(CS_LASER, HIGH);
  SPI.endTransaction();
}
