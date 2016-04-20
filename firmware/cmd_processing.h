/*
 * cmd_processing.h
 *
 *  Created on: 07.06.2015
 *      Author: schmelly
 */

#ifndef CMD_PROCESSING_H_
#define CMD_PROCESSING_H_

#include "cmd_handling.h"
#include "gcode_parser.h"

#ifdef __cplusplus
extern "C" {
#endif

void setupCommandProcessing();
void processCommand(struct gCode* code);
void renderPointCloud();

#ifdef __cplusplus
}
#endif

#endif /* CMD_PROCESSING_H_ */
