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
  PARSE_TOKEN, NEWLINE, LINE_NO, GCODE, MCODE, X_COORD, Y_COORD, Z_COORD, E_COORD, F_COORD, COMMENT
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
struct gCode g = { UNKNOWN, '\0', INT_MIN, FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN, INT_MIN };

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
      switch (code) {
      case 0:
        g.code = G0;
        break;
      case 1:
        g.code = G1;
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
      switch (code) {
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

void reset() {
  g.code = UNKNOWN;
  g.gCodeCharacter = '\0';
  g.gCodeDigit = INT_MIN;
  g.xCoord = FLT_MIN;
  g.yCoord = FLT_MIN;
  g.zCoord = FLT_MIN;
  g.eCoord = FLT_MIN;
  g.fCoord = FLT_MIN;
  g.lineNo = INT_MIN;
}