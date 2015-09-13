#ifndef ANYWAREEASING_H_
#define ANYWAREEASING_H_

#include "./Easing.h"
#include "anyware_colors.h"
#include "colorutils.h"

struct AnywareEasing {
  enum EasingType {
    BINARY,
    LINEAR,
    SINE,
    IN_CUBIC,
    OUT_CUBIC,
    PULSE,
    POP,
    BINARY_PULSE
  };

  bool active;
  uint8_t currval;
  uint8_t type;
  uint32_t startTime;
  uint16_t duration;
  
  AnywareEasing() : active(false), currval(0) {  }

  static bool getEasing(const char *easingstr, AnywareEasing::EasingType &easing);

  virtual void start(uint8_t type);
  virtual uint32_t end();
  virtual uint8_t apply(uint32_t t);
};

struct ColorEasing : public AnywareEasing {
  CRGB fromColor;
  CRGB toColor;
  
  ColorEasing() : AnywareEasing() {  }

  void start(uint8_t type, const CRGB &fromColor, const CRGB &toColor) {
    AnywareEasing::start(type);
    this->fromColor = fromColor;
    this->toColor = toColor;
  }

  CRGB applyColor(uint32_t t) {
    uint8_t val = apply(t);
    return blend(fromColor, toColor, val);
  }
};

#endif
