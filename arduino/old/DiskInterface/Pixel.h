#ifndef PIXEL_H_
#define PIXEL_H_

#include "anyware_global.h"
#include "AnywareEasing.h"

struct Pixel {

  Pixel(uint8_t strip, uint8_t panel)
    : strip(strip), panel(panel), easingid(-1) {}

  void ease(AnywareEasing::EasingType type, const CRGB &toColor, CRGB &buf);
  bool applyEasing(CRGB &buf);

  uint8_t strip;
  uint8_t panel;
  int8_t easingid;
};

#endif
