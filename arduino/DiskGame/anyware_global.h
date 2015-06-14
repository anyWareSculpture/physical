#ifndef ANYWARE_GLOBAL_H_
#define ANYWARE_GLOBAL_H_

#include <Arduino.h>

// Set to 1 to start in debug mode
#define DEBUG_MODE 1

extern int global_state;
extern int global_userid;
extern bool global_debug;

void setupCommonCommands();
int getUserIdArg(const char *arg);

void setupCommands();
void reset(bool);
uint32_t Color(uint8_t r, uint8_t g, uint8_t b);
void printError(const char *type, const char *msg);

void handleSerial();

#endif
