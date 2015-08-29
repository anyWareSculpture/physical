#ifndef NEOPIXEL_H_
#define NEOPIXEL_H_

#include "Arduino.h"
#include "AnywareEasing.h"
#include "./Adafruit_NeoPixel.h"

extern Adafruit_NeoPixel pixels;

class NeoPixel {
public:
  NeoPixel(int8_t pixelid)
    : pixel(pixelid), easingid(-1) {}

  void setup() {
  }
  
  void setColor(uint32_t col) {
    if (pixel >= 0) pixels.setPixelColor(pixel, col);
  }

  uint32_t getColor() {
    return pixel >= 0 ? pixels.getPixelColor(pixel) : 0;
  }

  void ease(AnywareEasing::EasingType type, uint32_t toColor);
  bool applyEasing();

public:
  int8_t pixel;
  int8_t easingid;
};

#endif
