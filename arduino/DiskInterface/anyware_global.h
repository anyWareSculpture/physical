#ifndef ANYWARE_GLOBAL_H_
#define ANYWARE_GLOBAL_H_

#include <Arduino.h>
#include <avr/pgmspace.h>

extern int global_state;
extern int global_userid;
extern bool global_debug;

void reset(bool);

void setupIR();

#endif
