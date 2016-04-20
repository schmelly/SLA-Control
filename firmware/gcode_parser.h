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
  UNKNOWN,

  G0, // set axis position (laser off)
  G1, // set axis position (laser on)
  G28, // home all axis
  G92,
  G161, // home axes to minimum, e.g. G161 X1
  G162, // home axes to maximum, e.g. G162 X1
  M0, // pause for x micros, e.g. M0 P1000
  M12, // set laser intensity, e.g. M12 S256
  M13, // enable laser M13 S1
  M14, // configure line segmentation values, e.g. M13 X5 S66 (5 seg length, 66 steps per segment)
  M15, // configure timing (spot exposure, resing settle), e.g. M15 S5000 P150 (5s settle, 0.15ms exposure)
  M16, // configure dip depth and time, e.g. M16 Z4 S3000 (4mm dip depth, 3s dip time)
  M105,
  M110,
  M208, // set axis max dimensions, e.g. M208 S1 X100 (minX) or M208 X100 (maxX)
  M500, // store parameters in flash memory
  M501, // read parameters from flash memory
  M502, // revert to default values in config.h
  M503, // print loaded settings
  M556, // axis compensation
  M710 // render point cloud for error calibration, e.g. M710 S1 P1 (rendering on, with error compensation) or M710 S1 (rendering on, without error compensation) or M710 S0 (rendering off)
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
  int lineNo;
  int pValue;
  int sValue;
};

gCode* parseGCodeLine(char* line);
gCode* getParsedGCode();

#ifdef __cplusplus
}
#endif

#endif /* GCODE_PARSER_H_ */
