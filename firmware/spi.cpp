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

const word maskFirstByteA = 0b00110000;
const word maskSecondByteA = 0b0000000011111111;

const word maskFirstByteB = 0b10110000;
const word maskSecondByteB = 0b0000000011111111;

void setupSPI() {

  pinMode(CS_MOTORS, OUTPUT);
  digitalWrite(CS_MOTORS, HIGH);

  SPI.begin(CS_MOTORS);
  SPI.setBitOrder(CS_MOTORS, MSBFIRST);
  SPI.setDataMode(CS_MOTORS, SPI_MODE0);
  SPI.setClockDivider(CS_MOTORS, SPI_CLOCK_DIV8);

  pinMode(CS_LASER, OUTPUT);
  digitalWrite(CS_LASER, HIGH);

  SPI.begin(CS_LASER);
  SPI.setBitOrder(CS_LASER, MSBFIRST);
  SPI.setDataMode(CS_LASER, SPI_MODE0);
  SPI.setClockDivider(CS_LASER, SPI_CLOCK_DIV8);

  changeMotorValues(DAC_ZERO, DAC_ZERO);
  changeLaserValue(0);
}

void changeMotorValues(int xValue, int yValue) {

#ifdef INVERT_X
  xValue = DAC_MAX-xValue;
#endif
#ifdef INVERT_Y
  yValue = DAC_MAX-yValue;
#endif

  byte a1 = maskFirstByteA | ((xValue & 0xFF00) >> 8);
  byte a2 = maskSecondByteA & xValue;

  byte b1 = maskFirstByteB | ((yValue & 0xFF00) >> 8);
  byte b2 = maskSecondByteB & yValue;

  SPI.transfer(CS_MOTORS, a1, SPI_CONTINUE);
  SPI.transfer(CS_MOTORS, a2, SPI_LAST);

  SPI.transfer(CS_MOTORS, b1, SPI_CONTINUE);
  SPI.transfer(CS_MOTORS, b2, SPI_LAST);
}

void changeLaserValue(int value) {

  byte a1 = maskFirstByteA | ((value & 0xFF00) >> 8);
  byte a2 = maskSecondByteA & value;

  SPI.transfer(CS_LASER, a1, SPI_CONTINUE);
  SPI.transfer(CS_LASER, a2, SPI_LAST);
}
