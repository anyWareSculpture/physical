#include "LEDStrip.h"

LEDStripInterface *LEDStripInterface::LEDStrips[MAX_STRIPS];
uint8_t LEDStripInterface::numStrips = 0;
Pair LEDStripInterface::mapping[MAX_STRIPS][MAX_PANELS];
