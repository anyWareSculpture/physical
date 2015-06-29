#ifndef DISKGAME_H_
#define DISKGAME_H_

#include "anyware_global.h"

// State flags
#define STATE_IDLE      0x00
#define STATE_DISK      0x01

extern int global_state;

enum DirectionFlags {
  DIR_OFF = 0x00,
  DIR_CCW = 0x01,
  DIR_CW =  0x02,
  DIR_BLOCKED = 0x03
};

void do_disk_init();
void do_disk_exit();
void do_disk_reset();
void do_disk_state();
void do_disk(uint8_t diskid, int userid, int pos, DirectionFlags dir);

#endif
