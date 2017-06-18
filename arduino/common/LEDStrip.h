#ifndef LEDSTRIP_H_
#define LEDSTRIP_H_

#include "Pixel.h"
#include "AnywareEasing.h"
#include "./FastLED.h"

struct Pair {
  Pair(uint8_t stripid = -1, uint8_t pixelid = -1) : stripid(stripid), pixelid(pixelid) {}
  int8_t stripid;
  int8_t pixelid;
};

class LEDStripInterface {
public:
  LEDStripInterface(uint8_t numpixels, Pixel *pixels) : numpixels(numpixels), pixels(pixels) {
    stripid = numStrips++;
    LEDStrips[stripid] = this;
  }

  virtual void setup() {
    for (uint8_t i=0;i<numpixels;i++) {
      mapping[pixels[i].strip][pixels[i].panel] = Pair(stripid,i);
    }
  }

  uint8_t getNumPixels() {
    return numpixels;
  }

  Pixel &getPixel(uint8_t pixelid) {
    return pixels[pixelid];
  }

  static LEDStripInterface &getStrip(uint8_t stripid) {
    return *LEDStrips[stripid];
  }

  static uint8_t getNumStrips() {
    return numStrips;
  }

  static const Pair &mapToLED(uint8_t stripid, uint8_t panelid) {
    return mapping[stripid][panelid];
  }

  static void setAllColors(const CRGB &col) {
    for (uint8_t s=0;s<numStrips;s++) {
      LEDStripInterface &strip = *LEDStrips[s];
      for (int i = 0; i < strip.getNumPixels(); i++) strip.setColor(i, col);
    }
    FastLED.show(); // This sends the updated pixel color to the hardware. 
  }
  
  virtual void setColor(uint8_t pixel, const CRGB &col) = 0;
  virtual const CRGB &getColor(uint8_t pixel) = 0;

  virtual void ease(uint8_t pixel, AnywareEasing::EasingType type, const CRGB &toColor, uint16_t duration = 0) = 0;
  virtual bool applyEasing() = 0;

  static void applyEasings() {
    bool changed = false;
    for (uint8_t s=0;s<numStrips;s++) {
      LEDStripInterface &strip = *LEDStrips[s];
      changed |= strip.applyEasing();
    }
    if (changed) FastLED.show(); // This sends the updated pixel color to the hardware. 
  }

  static LEDStripInterface *LEDStrips[MAX_STRIPS];
  static uint8_t numStrips;

  //  logical strip, logical panel => physical strip, physical pixel
  static Pair mapping[MAX_STRIPS][MAX_PANELS];

protected:
  uint8_t stripid;
  uint8_t numpixels;
  Pixel *pixels;
};

template<uint8_t dataPin, uint8_t clockPin> class LEDStrip : public LEDStripInterface {
public:
  LEDStrip(uint8_t numpixels, CRGB *leds, Pixel *pixels)
    : LEDStripInterface(numpixels, pixels), leds(leds) {
  }

  virtual void setup() {
    LEDStripInterface::setup();
    FastLED.addLeds<APA102, dataPin, clockPin, BGR, DATA_RATE_KHZ(100)>(leds, getNumPixels());
  }

  virtual void setColor(uint8_t pixelid, const CRGB &col) {
    pixels[pixelid].cancelEasing(leds[pixelid]);
    leds[pixelid] = col;
  }

  virtual const CRGB &getColor(uint8_t pixelid) {
    return leds[pixelid];
  }

  virtual void ease(uint8_t pixelid, AnywareEasing::EasingType type, const CRGB &toColor, uint16_t duration = 0) {
    pixels[pixelid].ease(type, toColor, leds[pixelid], duration);
  }

  virtual bool applyEasing() {
    bool changed = false;
    for (uint8_t i=0;i<numpixels;i++) {
      changed |= pixels[i].applyEasing(leds[i]);
    }
    return changed;
  }

  CRGB *leds;
};

#endif
