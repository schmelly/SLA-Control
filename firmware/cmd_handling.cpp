/*
 * cmd_handling.cpp
 *
 *  Created on: 07.06.2015
 *      Author: schmelly
 */

#include "Arduino.h"
#include "cmd_handling.h"
#include "gcode_parser.h"

#include <limits.h>
#include <float.h>

//#include "SerialCommand.h"
//#include "spi.h"

char lineBuffer[128];

//SerialCommand sCmd;
//enum commands currentCmd = UNKNOWN;
//
//float xyCoords[2] = { 0.0f, 0.0f };
//float zCoord = 0.0f;
//int laser = 0;
//
//String parsedComment = "";
//
//void onMove();
//void onLinearMove(String p);
//void onZMove(String p);
//void onComment();
//void onArbitraryGCode();
//void onUnknownCmd();
//void onLaser();
//
//void setupCmdHandling() {
//
////  sCmd.addCommand("G1", onMove);
////  sCmd.addCommand("G", onArbitraryGCode);
////  sCmd.addCommand("M110", onArbitraryGCode);
////  sCmd.addCommand("M105", onArbitraryGCode);
////  sCmd.addCommand("N", onArbitraryGCode);
////  sCmd.addCommand("L", onLaser);
////  sCmd.addCommand(";", onComment);
////  sCmd.addDefaultHandler(onUnknownCmd);
//}

void sendInitComplete() {

  Serial.println("start");
}

void printCode(gCode* code) {

  char msg[100];

  memset(msg, '\0', sizeof(char) * 100);
  sprintf(msg, "code: %d, gCodeCharacter: %c, gCodeDigit: %d", code->code, code->gCodeCharacter == '\0' ? '_' : code->gCodeCharacter, code->gCodeDigit == INT_MIN ? 0 : code->gCodeDigit);
  Serial.println(msg);

  memset(msg, '\0', sizeof(char) * 100);
  sprintf(msg, "xCoord: %.4f, yCoord: %.4f, zCoord: %.4f, eCoord: %.4f, fCoord: %.4f", code->xCoord == -FLT_MAX ? 0.f : code->xCoord, code->yCoord == -FLT_MAX ? 0.f : code->yCoord,
      code->zCoord == -FLT_MAX ? 0.f : code->zCoord, code->eCoord == -FLT_MAX ? 0.f : code->eCoord, code->fCoord == -FLT_MAX ? 0.f : code->fCoord);
  Serial.println(msg);

  memset(msg, '\0', sizeof(char) * 100);
  sprintf(msg, "laserIntensity: %d, lineNo: %d\n", code->laserIntensity, code->lineNo);
  Serial.println(msg);
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

//  char msg[128];
//
//  memset(msg, '\0', sizeof(char) * 128);
//  sprintf(msg, "lineBuffer: %s", lineBuffer);
//  Serial.println(msg);
//
//  gCode* code =
  parseGCodeLine(lineBuffer);
//  printCode(code);

  return 1;
}

//enum commands getCurrentCmd() {
//
//  return currentCmd;
//}
//
//float* getXYCoords() {
//  return xyCoords;
//}
//
//float getZCoord() {
//  return zCoord;
//}
//
//int getLaser() {
//  return laser;
//}
//
//void onMove() {
//
//  char* argument = sCmd.next();
//  if (!argument) {
//    currentCmd = UNKNOWN;
//    return;
//  }
//
//  String s = "";
//  s += argument;
//  int length = s.length();
//  if (length >= 2 && s.startsWith("X")) {
//    onLinearMove(s);
//    return;
//  } else if (length >= 2 && s.startsWith("Z")) {
//    onZMove(s);
//    return;
//  } else {
//    currentCmd = UNKNOWN;
//    return;
//  }
//}
//
//void onLinearMove(String p) {
//
//  String s = p;
//  s = s.substring(1);
//  xyCoords[0] = s.toFloat();
//
//  char* argument = sCmd.next();
//  if (!argument) {
//    currentCmd = UNKNOWN;
//    return;
//  }
//
//  s = "";
//  s += argument;
//  int length = s.length();
//  if (length < 2 || !s.startsWith("Y")) {
//    currentCmd = UNKNOWN;
//    return;
//  }
//
//  s = s.substring(1);
//  xyCoords[1] = s.toFloat();
//
//  currentCmd = LINEAR_MOVE;
//}
//
//void onZMove(String p) {
//
//  String s = p;
//  s = s.substring(1);
//  zCoord = s.toFloat();
//
//  currentCmd = Z_MOVE;
//}
//
//void onComment() {
//
//  parsedComment = "";
//  char* token;
//  while ((token = sCmd.next())) {
//    parsedComment += token;
//    parsedComment += " ";
//  }
//  currentCmd = COMMENT;
//}
//
//void onArbitraryGCode() {
//
//  currentCmd = ARBITRARY_GCODE;
//}
//
//void onUnknownCmd() {
//
//  Serial.println("received unknown command");
//}
//
//void onLaser() {
//
//  char* argument = sCmd.next();
//  if (!argument) {
//    return;
//  }
//
//  String s = "";
//  s += argument;
//  int length = s.length();
//  if (length < 1) {
//    return;
//  }
//
//  laser = s.toInt();
//
//  currentCmd = LASER;
//}
