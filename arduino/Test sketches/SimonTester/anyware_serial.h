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

#endif
