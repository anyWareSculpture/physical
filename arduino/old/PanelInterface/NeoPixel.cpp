#include "NeoPixel.h"
#include "anyware_serial.h"

// We don't have enough memory to have one easing per pixel, so we have to reuse from a pool of easings
#define NUM_EASINGS 5
ColorEasing easings[NUM_EASINGS];

void NeoPixel::ease(AnywareEasing::EasingType type, uint32_t toColor)
{
  // If we're already easing, end the easing and start another one
  if (easingid >= 0) {
    uint32_t val = easings[easingid].end();
    pixels.setPixelColor(pixel, val);
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
    easings[easingid].start(type, pixels.getPixelColor(pixel), toColor);
    Serial.print("Found easing: "); Serial.println(easingid);
  }
  else {
    printError(F("internal error"), F("No easings available"));
  }
}

bool NeoPixel::applyEasing()
{
  bool retval = false;
  if (easingid >=0 && easings[easingid].active) {
    uint8_t oldval = easings[easingid].currval;
    uint32_t col = easings[easingid].applyColor(millis());
    if (oldval != easings[easingid].currval) {
      pixels.setPixelColor(pixel, col);
      retval = true;
    }
    if (!easings[easingid].active) easingid = -1;
  }
  return retval;
}
