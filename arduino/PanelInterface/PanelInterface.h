#ifndef PANELINTERFACE_H_
#define PANELINTERFACE_H_

#include "AnywareEasing.h"
#include "PanelSet.h"

void do_panel_init();
void do_panel_exit();
void do_panel_set(uint8_t strip, PanelSet panels, uint8_t intensity, const CRGB &color, AnywareEasing::EasingType  easing);
void do_panel_pulse(uint8_t strip, PanelSet panels, uint8_t intensity, const CRGB &color, AnywareEasing::EasingType  easing);
void do_panel_intensity(uint8_t strip, uint8_t intensity);

#endif
