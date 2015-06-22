#ifndef PANELINTERFACE_H_
#define PANELINTERFACE_H_

#include "AnywareEasing.h"

// How many Panels are attached to the Arduino?
#define NUMPANELS 30

// State flags
#define STATE_IDLE      0x00
#define STATE_PLAYBACK  0x01
#define STATE_READY     0x02
#define STATE_SUCCESS   0x03
#define STATE_FAILURE   0x04

extern int global_state;

void do_panel_init();
void do_panel_exit();
void do_panel_set(uint8_t strip, uint8_t panel, uint8_t intensity, uint32_t color, AnywareEasing::EasingType  easing);
void do_panel_pulse(uint8_t strip, uint8_t panel, uint8_t intensity, uint32_t color, AnywareEasing::EasingType  easing);
void do_panel_intensity(uint8_t strip, uint8_t intensity);
void do_panel_animate();
void do_panel_state(int state);

#endif
