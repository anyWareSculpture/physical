#ifndef ANYWARE_GLOBAL_H_
#define ANYWARE_GLOBAL_H_

#include <Arduino.h>
#include <avr/pgmspace.h>

extern bool global_initialized;
extern int global_state;
extern int global_userid;
extern bool global_debug;

// Called after performing the RESET command. Must be implemented by each interface.
void resetInterface(bool);
// Called after performing the INIT command. Must be implemented by each interface.
void initInterface();

void setupIR();

#endif
