#ifndef ANYWARE_COLORS_H_
#define ANYWARE_COLORS_H_

#include "Arduino.h"
#include "pixeltypes.h"

extern const CRGB locationColor[3];

void getRGB(const CRGB &col, uint8_t &r, uint8_t &g, uint8_t &b);
CRGB scaleColor(const CRGB &from, const CRGB &to, uint8_t ratio);
CRGB applyIntensity(const CRGB &, uint8_t intensity);
bool getColor(const char *colorstr, CRGB &col);

// Colors
const CRGB RED(255,0,0);
const CRGB GREEN(0,255,0);
const CRGB BLACK(0,0,0);
const CRGB WHITE(255, 255, 255);
const CRGB MYBLUE(0,20,147);
const CRGB MYYELLOW (255, 255, 0);
const CRGB MYPINK(255,20,147);

#endif
