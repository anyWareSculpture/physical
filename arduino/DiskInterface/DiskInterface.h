#ifndef DISKINTERFACE_H_
#define DISKINTERFACE_H_

#include "AnywareEasing.h"

// State flags
const int STATE_BLOCKED = -1;
const int STATE_READY   = 0;
const int STATE_HOMING  = 1;

extern int global_state;

enum Direction {
  DIR_CW =  -1,
  DIR_OFF =  0,
  DIR_CCW =  1
};

void do_disk_init();
void do_disk_exit();
void do_disk_reset();
void do_disk(uint8_t diskid, int userid, int pos, Direction dir);
void do_panel_set(uint8_t strip, uint8_t panel, uint8_t intensity, const CRGB &color, AnywareEasing::EasingType  easing);
void do_panel_pulse(uint8_t strip, uint8_t panel, uint8_t intensity, const CRGB &color, AnywareEasing::EasingType  easing);
void do_panel_intensity(uint8_t strip, uint8_t intensity);

#endif
