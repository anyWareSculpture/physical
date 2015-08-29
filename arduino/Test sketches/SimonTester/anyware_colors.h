#ifndef ANYWARE_COLORS_H_
#define ANYWARE_COLORS_H_

#include "Arduino.h"

extern const uint32_t locationColor[3];

uint32_t Color(uint8_t r, uint8_t g, uint8_t b);
void getRGB(uint32_t col, uint8_t &r, uint8_t &g, uint8_t &b);
uint32_t scaleColor(uint32_t from, uint32_t to, uint8_t ratio);
uint32_t applyIntensity(uint32_t col, uint8_t intensity);
bool getColor(const char *colorstr, uint32_t &col);

// Colors
const uint32_t RED = Color(255,0,0);
const uint32_t GREEN = Color(0,255,0);
const uint32_t BLACK = Color(0,0,0);
const uint32_t WHITE = Color(255, 255, 255);
const uint32_t MYBLUE = Color(0,20,147);
const uint32_t MYYELLOW = Color(255, 255, 0);
const uint32_t MYPINK = Color(255,20,147);

#endif
