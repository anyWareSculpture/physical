#ifndef ANYWARE_GLOBAL_H_
#define ANYWARE_GLOBAL_H_

#include <Arduino.h>
#include <avr/pgmspace.h>
#include "anyware_error.h"

#define MAX_STRIPS 10
#define MAX_PANELS 10

#define HELLO_BLACKOUT_TIME 100

extern int global_state;
extern bool global_debug;

// Called after performing the RESET command. Must be implemented by each interface.
void resetInterface(bool);
// Called after performing the INIT command. Must be implemented by each interface.
void initInterface();

#if 0 // Not used any longer
void setupIR();
#endif

#endif
