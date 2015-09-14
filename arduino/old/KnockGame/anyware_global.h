#ifndef ANYWARE_GLOBAL_H_
#define ANYWARE_GLOBAL_H_

#include <Arduino.h>

// Set to 1 to start in debug mode
#define DEBUG_MODE 1

uint32_t Color(uint8_t r, uint8_t g, uint8_t b);

// Colors
const uint32_t RED = Color(255,0,0);
const uint32_t GREEN = Color(0,255,0);
const uint32_t BLACK = Color(0,0,0);
const uint32_t WHITE = Color(255, 255, 255);
const uint32_t MYBLUE = Color(0,20,147);
const uint32_t MYYELLOW = Color(255, 255, 0);
const uint32_t MYPINK = Color(255,20,147);

extern const uint32_t locationColor[3];

extern int global_state;
extern int global_userid;
extern bool global_debug;

void setupCommonCommands();
int getUserIdArg(const char *arg);

void setupCommands();
void reset(bool);
void printError(const char *type, const char *msg);

void handleSerial();
void setupIR();

#endif
