/*
 * cmd_handling.h
 *
 *  Created on: 07.06.2015
 *      Author: schmelly
 */

#ifndef CMD_HANDLING_H_
#define CMD_HANDLING_H_

#ifdef __cplusplus
extern "C" {
#endif

enum commands {
  UNKNOWN, ARBITRARY_GCODE, COMMENT, LINEAR_MOVE, Z_MOVE, LASER
};

void setupCmdHandling();
void sendInitComplete();
void nextCommand();
enum commands getCurrentCmd();
float* getXYCoords();
float getZCoord();
int getLaser();

#ifdef __cplusplus
}
#endif

#endif /* CMD_HANDLING_H_ */
