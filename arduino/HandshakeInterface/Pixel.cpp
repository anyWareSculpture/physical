#include "Pixel.h"
#include "anyware_global.h"

// We don't have enough memory to have one easing per pixel, so we have to reuse from a pool of easings
#define NUM_EASINGS 5
static ColorEasing easings[NUM_EASINGS];

void Pixel::ease(AnywareEasing::EasingType type, const CRGB &toColor, CRGB &buf)
{
  // If we're already easing, end the easing and start another one
  if (easingid >= 0) {
    uint32_t val = easings[easingid].end();
    buf = val;
    easingid = -1;
  }

  // Find available easing
  for (uint8_t i=0;i<NUM_EASINGS;i++) {
    if (!easings[i].active) {
      easingid = i;
      break;
    }
  }
  if (easingid >= 0) {
    easings[easingid].start(type, buf, toColor);
    if (global_debug) {
      Serial.print("DEBUG Found easing: "); Serial.println(easingid);
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
  if (easingid >=0 && easings[easingid].active) {
    uint8_t oldval = easings[easingid].currval;
    CRGB col = easings[easingid].applyColor(millis());
    if (oldval != easings[easingid].currval) {
      buf = col;
      retval = true;
    }
    if (!easings[easingid].active) easingid = -1;
  }
  return retval;
}
