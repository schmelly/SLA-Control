/*
 * spi.h
 *
 *  Created on: 23.07.2015
 *      Author: schmelly
 */

#ifndef SPI_H_
#define SPI_H_

#ifdef __cplusplus
extern "C" {
#endif

void setupSPI();
void changeMotorValues(int xValue, int yValue);
void changeLaserValue(int value);

#ifdef __cplusplus
}
#endif

#endif /* SPI_H_ */
