/*
 * gcode_parser.h
 *
 *  Created on: 31.10.2015
 *      Author: schmelly
 */

#ifndef GCODE_PARSER_H_
#define GCODE_PARSER_H_

#ifdef __cplusplus
extern "C" {
#endif

enum gCodes {
  UNKNOWN, G0, G1, G28, G92, L, M105, M110, S
};

struct gCode {
  gCodes code;
  char gCodeCharacter;
  int gCodeDigit;
  float xCoord;
  float yCoord;
  float zCoord;
  float eCoord;
  float fCoord;
  int laserIntensity;
  int switchLaser;
  int lineNo;
};

gCode* parseGCodeLine(char* line);
gCode* getParsedGCode();

#ifdef __cplusplus
}
#endif

#endif /* GCODE_PARSER_H_ */
