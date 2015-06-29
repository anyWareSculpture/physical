#include "NeoPixel.h"
#include "anyware_serial.h"

// We don't have enough memory to have one easing per pixel, so we have to reuse from a pool of easings
#define NUM_EASINGS 5
ColorEasing easings[NUM_EASINGS];

void NeoPixel::ease(AnywareEasing::EasingType type, uint32_t toColor)
{
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
  if (easingid >=0 && easings[easingid].active) {
    uint32_t val = easings[easingid].apply();
    pixels.setPixelColor(pixel, val);
    if (!easings[easingid].active) easingid = -1;
    return true;
  }
  return false;
}
