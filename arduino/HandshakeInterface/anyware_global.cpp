#include "Arduino.h"
#include "anyware_global.h"

int global_state;
bool global_debug = false;

#if 0 // Not used any longer
void setupIR () {
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  // Arduino MEGA: Pin9 is OC2B
  pinMode(9, OUTPUT);  //IR LED output
#else
  // Other Arduinos: Pin3 is OC2B
  pinMode(3, OUTPUT);  //IR LED output
#endif
  TCCR2A = _BV(COM2B0) | _BV(WGM21);
  TCCR2B = _BV(CS20);
  OCR2A = 209;
}
#endif
