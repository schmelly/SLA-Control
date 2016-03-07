/*
 * gcode_parser.cpp
 *
 *  Created on: 31.10.2015
 *      Author: schmelly
 */

#include "gcode_parser.h"
#include <limits.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

void reset();

enum parser_states {
  PARSE_TOKEN, NEWLINE, LINE_NO, GCODE, MCODE, X_COORD, Y_COORD, Z_COORD, E_COORD, F_COORD, LASER_INTENSITY, SWITCH_LASER, P_VALUE, COMMENT
};

//struct gCode {
//  gCodes code;
//  float xCoord;
//  float yCoord;
//  float zCoord;
//  float eCoord;
//  float fCoord;
//  int lineNo;
//};
struct gCode g = { UNKNOWN, '\0', INT_MIN, -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX, INT_MIN, 0, INT_MIN, INT_MIN };

gCode* parseGCodeLine(char* line) {

  char c;
  char *pEnd;
  int code;

  reset();
  enum parser_states curState = PARSE_TOKEN;

  while (curState != NEWLINE) {
    switch (curState) {

    ///////////////////////
    case PARSE_TOKEN:
      c = *line;
      switch (c) {
      case 'N':
        curState = LINE_NO;
        break;
      case 'G':
        curState = GCODE;
        break;
      case 'M':
        curState = MCODE;
        break;
      case 'X':
        curState = X_COORD;
        break;
      case 'Y':
        curState = Y_COORD;
        break;
      case 'Z':
        curState = Z_COORD;
        break;
      case 'E':
        curState = E_COORD;
        break;
      case 'F':
        curState = F_COORD;
        break;
      case 'L':
        curState = LASER_INTENSITY;
        break;
      case 'S':
        curState = SWITCH_LASER;
        break;
      case 'P':
        curState = P_VALUE;
        break;
      case ';':
        curState = COMMENT;
        break;
      case '\r':
        curState = NEWLINE;
        break;
      default:
        break;
      }
      if (curState != NEWLINE) {
        line++;
      }
      break;
      ///////////////////////

      ///////////////////////
    case NEWLINE:
      break;
      ///////////////////////

      ///////////////////////
    case LINE_NO:
      g.lineNo = (int) strtol(line, &pEnd, 10);
      line = pEnd;
      curState = PARSE_TOKEN;
      break;
      ///////////////////////

      ///////////////////////
    case GCODE:
      code = (int) strtol(line, &pEnd, 10);
      g.gCodeCharacter = 'G';
      g.gCodeDigit = code;
      switch (code) {
      case 1:
        g.code = G1;
        break;
      case 0:
        g.code = G0;
        break;
      case 28:
        g.code = G28;
        break;
      case 92:
        g.code = G92;
        break;
      default:
        g.code = UNKNOWN;
        break;
      }
      line = pEnd;
      curState = PARSE_TOKEN;
      break;
      ///////////////////////

      ///////////////////////
    case MCODE:
      code = (int) strtol(line, &pEnd, 10);
      g.gCodeCharacter = 'M';
      g.gCodeDigit = code;
      switch (code) {
      case 0:
        g.code = M0;
        break;
      case 105:
        g.code = M105;
        break;
      case 110:
        g.code = M110;
        break;
      default:
        g.code = UNKNOWN;
        break;
      }
      line = pEnd;
      curState = PARSE_TOKEN;
      break;
      ///////////////////////

      ///////////////////////
    case X_COORD:
      g.xCoord = (float) strtod(line, &pEnd);
      line = pEnd;
      curState = PARSE_TOKEN;
      break;
      ///////////////////////

      ///////////////////////
    case Y_COORD:
      g.yCoord = (float) strtod(line, &pEnd);
      line = pEnd;
      curState = PARSE_TOKEN;
      break;
      ///////////////////////

      ///////////////////////
    case Z_COORD:
      g.zCoord = (float) strtod(line, &pEnd);
      line = pEnd;
      curState = PARSE_TOKEN;
      break;
      ///////////////////////

      ///////////////////////
    case E_COORD:
      g.eCoord = (float) strtod(line, &pEnd);
      line = pEnd;
      curState = PARSE_TOKEN;
      break;
      ///////////////////////

      ///////////////////////
    case F_COORD:
      g.fCoord = (float) strtod(line, &pEnd);
      line = pEnd;
      curState = PARSE_TOKEN;
      break;
      ///////////////////////

      ///////////////////////
    case LASER_INTENSITY:
      g.code = L;
      g.gCodeCharacter = 'L';
      g.laserIntensity = (int) strtol(line, &pEnd, 10);
      line = pEnd;
      curState = PARSE_TOKEN;
      break;
      ///////////////////////

      ///////////////////////
    case SWITCH_LASER:
      g.code = S;
      g.gCodeCharacter = 'S';
      g.switchLaser = (int) strtol(line, &pEnd, 10);
      line = pEnd;
      curState = PARSE_TOKEN;
      break;
      ///////////////////////

      ///////////////////////
    case P_VALUE:
      g.pValue = (int) strtol(line, &pEnd, 10);
      line = pEnd;
      curState = PARSE_TOKEN;
      break;
      ///////////////////////

      ///////////////////////
    case COMMENT:
      do {
        line++;
        c = *line;
      } while (c != '\r' && c != '\0');
      curState = NEWLINE;
      break;
      ///////////////////////
    }
  }

  return &g;
}

gCode* getParsedGCode() {
  return &g;
}

void reset() {
  g.code = UNKNOWN;
  g.gCodeCharacter = '\0';
  g.gCodeDigit = INT_MIN;
  g.xCoord = -FLT_MAX;
  g.yCoord = -FLT_MAX;
  g.zCoord = -FLT_MAX;
  g.eCoord = -FLT_MAX;
  g.fCoord = -FLT_MAX;
  g.laserIntensity = INT_MIN;
  g.switchLaser = 0;
  g.lineNo = INT_MIN;
  g.pValue = INT_MIN;
}
