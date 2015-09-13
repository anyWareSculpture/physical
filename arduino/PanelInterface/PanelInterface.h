#ifndef PANELINTERFACE_H_
#define PANELINTERFACE_H_

#include "AnywareEasing.h"

// State flags
#define STATE_READY     0
#define STATE_SUCCESS   1
#define STATE_FAILURE   2

extern int global_state;

void do_panel_init();
void do_panel_exit();
void do_panel_set(uint8_t strip, uint8_t panel, uint8_t intensity, const CRGB &color, AnywareEasing::EasingType  easing);
void do_panel_pulse(uint8_t strip, uint8_t panel, uint8_t intensity, const CRGB &color, AnywareEasing::EasingType  easing);
void do_panel_intensity(uint8_t strip, uint8_t intensity);
void do_panel_animate();
void do_panel_state(int state);

#endif
