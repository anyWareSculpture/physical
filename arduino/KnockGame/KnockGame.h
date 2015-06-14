#ifndef KNOCKGAME_H_
#define KNOCKGAME_H_

#include "anyware_global.h"

// State flags
#define STATE_IDLE      0x00
#define STATE_HANDSHAKE 0x01
#define STATE_KNOCK     0x02

extern int global_state;

#define MAX_PATTERN_LENGTH 50
extern unsigned int knock_pattern[MAX_PATTERN_LENGTH];
extern uint8_t knock_pattern_count;

void do_knock_init();
void do_knock_exit();
void do_knock_pattern();
void do_knock_echo();

#endif
