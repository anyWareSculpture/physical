#ifndef HANDSHAKEINTERFACE_H_
#define HANDSHAKEINTERFACE_H_

#include "anyware_global.h"

// State flags
#define STATE_IDLE      0x00
#define STATE_HANDSHAKE 0x01

extern int global_state;

void do_handshake(bool active, uint8_t user);

#endif
