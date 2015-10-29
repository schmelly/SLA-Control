/*
 * cmd_handling.h
 *
 *  Created on: 07.06.2015
 *      Author: schmelly
 */

#ifndef CMD_HANDLING_H_
#define CMD_HANDLING_H_

enum commands {
  UNKNOWN, COMMENT, LINEAR_MOVE, Z_MOVE, LASER
};

void setupCmdHandling();
void sendInitComplete();
void nextCommand();
enum commands getCurrentCmd();
float* getXYCoords();
float getZCoord();
int getLaser();

#endif /* CMD_HANDLING_H_ */
