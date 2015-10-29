/*
 * cmd_handling.cpp
 *
 *  Created on: 07.06.2015
 *      Author: schmelly
 */

#include "cmd_handling.h"
#include "SerialCommand.h"
#include "spi.h"

SerialCommand sCmd;
enum commands currentCmd = UNKNOWN;

float xyCoords[2] = { 0.0f, 0.0f };
float zCoord = 0.0f;
int laser = 0;

String parsedComment = "";

void onMove();
void onLinearMove(String p);
void onZMove(String p);
void onComment();
void onUnknownCmd();
void onLaser();

void setupCmdHandling() {

  sCmd.addCommand("G1", onMove);
  sCmd.addCommand("L", onLaser);
  sCmd.addCommand(";", onComment);
  sCmd.addDefaultHandler(onUnknownCmd);
}

void sendInitComplete() {

  Serial.println("start");
}

void nextCommand() {

  currentCmd = UNKNOWN;
  sCmd.readSerial();
}

enum commands getCurrentCmd() {

  return currentCmd;
}

float* getXYCoords() {
  return xyCoords;
}

float getZCoord() {
  return zCoord;
}

int getLaser() {
  return laser;
}

void onMove() {

  char* argument = sCmd.next();
  if (!argument) {
    currentCmd = UNKNOWN;
    return;
  }

  String s = "";
  s += argument;
  int length = s.length();
  if (length >= 2 && s.startsWith("X")) {
    onLinearMove(s);
    return;
  } else if (length >= 2 && s.startsWith("Z")) {
    onZMove(s);
    return;
  } else {
    currentCmd = UNKNOWN;
    return;
  }
}

void onLinearMove(String p) {

  String s = p;
  s = s.substring(1);
  xyCoords[0] = s.toFloat();

  char* argument = sCmd.next();
  if (!argument) {
    currentCmd = UNKNOWN;
    return;
  }

  s = "";
  s += argument;
  int length = s.length();
  if (length < 2 || !s.startsWith("Y")) {
    currentCmd = UNKNOWN;
    return;
  }

  s = s.substring(1);
  xyCoords[1] = s.toFloat();

  currentCmd = LINEAR_MOVE;
}

void onZMove(String p) {

  String s = p;
  s = s.substring(1);
  zCoord = s.toFloat();

  currentCmd = Z_MOVE;
}

void onComment() {

  parsedComment = "";
  char* token;
  while ((token = sCmd.next())) {
    parsedComment += token;
    parsedComment += " ";
  }
  currentCmd = COMMENT;
}

void onUnknownCmd() {

  Serial.println("received unknown command");
}

void onLaser() {

  char* argument = sCmd.next();
  if (!argument) {
    return;
  }

  String s = "";
  s += argument;
  int length = s.length();
  if (length < 1) {
    return;
  }

  laser = s.toInt();

  currentCmd = LASER;
}
