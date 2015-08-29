#ifndef ANYWARESERIAL_H_
#define ANYWARESERIAL_H_

#include "anyware_global.h"
#include "./SerialCommand.h"

extern SerialCommand sCmd;

int getUserIdArg(const char *arg);

void setupCommands();
void setupCommonCommands();
void addCommand(const char *cmd, void(*function)(), const char *wildcard = NULL);
void handleSerial();

void printError(const char *type, const char *msg);
void printError(const __FlashStringHelper *type, const __FlashStringHelper *msg);
void printError(const __FlashStringHelper *type, const char *msg);

#endif
