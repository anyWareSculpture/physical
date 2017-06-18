#include "Pixel.h"
#include "anyware_global.h"

// We don't have enough memory to have one easing per pixel, so we have to reuse from a pool of easings
#define NUM_EASINGS 5
static ColorEasing easings[NUM_EASINGS];

void Pixel::cancelEasing(CRGB &buf)
{
  if (this->easingid >= 0) {
    uint32_t val = easings[this->easingid].end();
    buf = val;
    this->easingid = -1;
  }
}

void Pixel::ease(AnywareEasing::EasingType type, const CRGB &toColor, CRGB &buf, uint16_t duration)
{
  // If we're already easing, end the easing and start another one
  if (this->easingid >= 0) this->cancelEasing(buf);

  // Find available easing
  for (uint8_t i=0;i<NUM_EASINGS;i++) {
    if (!easings[i].active) {
      this->easingid = i;
      break;
    }
  }
  if (this->easingid >= 0) {
    easings[this->easingid].start(type, buf, toColor, duration);
    if (global_debug) {
      Serial.print("DEBUG Found easing: "); Serial.println(this->easingid);
    }
  }
  else {
    printError(F("internal error"), F("No easings available"));
    buf = toColor;
  }
}

bool Pixel::applyEasing(CRGB &buf)
{
  bool retval = false;
  if (this->easingid >=0 && easings[this->easingid].active) {
    uint8_t oldval = easings[this->easingid].currval;
    CRGB col = easings[this->easingid].applyColor(millis());
    if (oldval != easings[this->easingid].currval) {
      buf = col;
      retval = true;
    }
    if (!easings[this->easingid].active) {
      this->easingid = -1;
    }
  }
  return retval;
}
