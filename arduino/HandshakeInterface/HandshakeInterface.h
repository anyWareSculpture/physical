#ifndef HANDSHAKEINTERFACE_H_
#define HANDSHAKEINTERFACE_H_

#include "AnywareEasing.h"
#include "PanelSet.h"

void do_panel_set(uint8_t strip, PanelSet panels, uint8_t intensity, const CRGB &color, AnywareEasing::EasingType easing, uint16_t duration = 0);
void do_panel_pulse(uint8_t strip, PanelSet panels, uint8_t intensity, const CRGB &color, AnywareEasing::EasingType  easing, uint16_t duration = 0);
void do_panel_intensity(uint8_t strip, uint8_t intensity);
void do_handshake(uint8_t numusers);

#endif
