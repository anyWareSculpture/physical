#ifndef ANYWAREEASING_H_
#define ANYWAREEASING_H_

#include "./Easing.h"
#include "anyware_colors.h"

struct AnywareEasing {
  enum EasingType {
    BINARY,
    LINEAR,
    SINE,
    IN_CUBIC,
    OUT_CUBIC,
    PULSE,
    POP
  };

  bool active;
  uint8_t type;
  uint32_t startTime;
  uint16_t duration;
  
  AnywareEasing() : active(false) {  }

  static bool getEasing(const char *easingstr, AnywareEasing::EasingType &easing);

  void start(uint8_t type);
  uint8_t apply();
};

struct ColorEasing : public AnywareEasing {
  uint32_t fromColor;
  uint32_t toColor;
  
  ColorEasing() : AnywareEasing() {  }

  void start(uint8_t type, uint32_t fromColor, uint32_t toColor) {
    AnywareEasing::start(type);
    this->fromColor = fromColor;
    this->toColor = toColor;
  }

  uint32_t apply() {
    uint8_t val = AnywareEasing::apply();
    return scaleColor(fromColor, toColor, val);
  }
};

#endif
