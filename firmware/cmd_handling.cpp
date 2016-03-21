/*
 * cmd_handling.cpp
 *
 *  Created on: 07.06.2015
 *      Author: schmelly
 */

#include "Arduino.h"
#include "cmd_handling.h"
#include "gcode_parser.h"
#include "config.h"

#include <limits.h>
#include <float.h>

char lineBuffer[128];

void sendInitComplete() {

  Serial.println("start");
}

void printCode(gCode* code) {

  serialPrintln("code: %d, gCodeCharacter: %c, gCodeDigit: %d", code->code, code->gCodeCharacter == '\0' ? '_' : code->gCodeCharacter,
      code->gCodeDigit == INT_MIN ? 0 : code->gCodeDigit);

  serialPrintln("xCoord: %.4f, yCoord: %.4f, zCoord: %.4f, eCoord: %.4f, fCoord: %.4f", code->xCoord == -FLT_MAX ? 0.f : code->xCoord,
      code->yCoord == -FLT_MAX ? 0.f : code->yCoord, code->zCoord == -FLT_MAX ? 0.f : code->zCoord, code->eCoord == -FLT_MAX ? 0.f : code->eCoord,
      code->fCoord == -FLT_MAX ? 0.f : code->fCoord);

  serialPrintln("sValue: %d, pValue: %d, lineNo: %d\n", code->sValue, code->pValue, code->lineNo);
}

int nextCommand() {

  if (Serial.available() <= 0) {
    return 0;
  }

  memset(lineBuffer, '\0', sizeof(char) * 128);

  int i = 0;
  char c = ' ';
  char* p = lineBuffer;

  uint32_t timestamp = millis() + 1000;

  while (c != '\r' && c != '\0') {

    if ((int32_t)(millis() - timestamp) >= 0) {
      memset(lineBuffer, '\0', sizeof(char) * 128);
      return 0;
    }

    if (Serial.available() > 0) {
      c = Serial.read();
      *p = c;

      i++;
      if (i == 128) {
        memset(lineBuffer, '\0', sizeof(char) * 128);
        return 0;
      }
      p++;
    }
  }

//  gCode* code =
  parseGCodeLine(lineBuffer);
//  printCode(code);

  return 1;
}
