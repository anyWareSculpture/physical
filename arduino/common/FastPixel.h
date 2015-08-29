#ifndef NEOPIXEL_H_
#define NEOPIXEL_H_

#include "Arduino.h"
#include "AnywareEasing.h"
#include "./FastLED.h"

extern CRGB leds[];

class FastPixel {
public:
  FastPixel(int8_t pixelid)
    : pixel(pixelid), easingid(-1) {}

  void setup() {
  }
  
  void setColor(uint32_t col) {
    if (pixel >= 0) leds[pixel] = CRGB(col);
  }

  uint32_t colorCode(const CRGB &crgb) {
    return Color(crgb.r, crgb.g, crgb.b);
  }

  uint32_t getColor() {
    return pixel >= 0 ? colorCode(leds[pixel]) : 0;
  }

  void ease(AnywareEasing::EasingType type, uint32_t toColor);
  bool applyEasing();

public:
  int8_t pixel;
  int8_t easingid;
};

#endif
