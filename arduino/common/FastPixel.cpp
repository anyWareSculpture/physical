#include "FastPixel.h"
#include "anyware_global.h"

// We don't have enough memory to have one easing per pixel, so we have to reuse from a pool of easings
#define NUM_EASINGS 5
static ColorEasing easings[NUM_EASINGS];

void FastPixel::ease(AnywareEasing::EasingType type, uint32_t toColor)
{
  // If we're already easing, end the easing and start another one
  if (easingid >= 0) {
    uint32_t val = easings[easingid].end();
    leds[pixel] = val;
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
    easings[easingid].start(type, getColor(), toColor);
    if (global_debug) {
      Serial.print("DEBUG Found easing: "); Serial.println(easingid);
    }
  }
  else {
    printError(F("internal error"), F("No easings available"));
    leds[pixel] = toColor;
  }
}

bool FastPixel::applyEasing()
{
  bool retval = false;
  if (easingid >=0 && easings[easingid].active) {
    uint8_t oldval = easings[easingid].currval;
    uint32_t col = easings[easingid].applyColor(millis());
    if (oldval != easings[easingid].currval) {
      leds[pixel] = col;
      retval = true;
    }
    if (!easings[easingid].active) easingid = -1;
  }
  return retval;
}
