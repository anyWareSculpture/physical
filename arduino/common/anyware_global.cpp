#include "Arduino.h"
#include "anyware_global.h"

int global_state;
int global_userid = -1;
bool global_debug = false;

const uint32_t locationColor[3] = {
  MYBLUE,
  MYYELLOW,
  MYPINK
};

// Convert separate R,G,B into packed 32-bit RGB color.
// Packed format is always RGB, regardless of LED strand color order.
uint32_t Color(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
}

uint32_t scaleColor(uint32_t from, uint32_t to, uint8_t ratio) {
  uint8_t a[3] = {
    (uint8_t)(from >> 16),
    (uint8_t)(from >>  8),
    (uint8_t)from };

  uint8_t b[3] = {
    (uint8_t)(to >> 16),
    (uint8_t)(to >>  8),
    (uint8_t)to };

  uint32_t c = Color(uint8_t(a[0] + (b[0] - a[0]) * ratio / 255),
                     uint8_t(a[1] + (b[1] - a[1]) * ratio / 255),
                     uint8_t(a[2] + (b[2] - a[2]) * ratio / 255));

  return c;
}

void printError(const char *type, const char *msg)
{
  Serial.print(F("ERROR "));
  Serial.print(type);
  Serial.print(F(": "));
  Serial.println(msg);
}

void printError(const __FlashStringHelper *type, const __FlashStringHelper *msg)
{
  Serial.print(F("ERROR "));
  Serial.print(type);
  Serial.print(F(": "));
  Serial.println(msg);
}

void printError(const __FlashStringHelper *type, const char *msg)
{
  Serial.print(F("ERROR "));
  Serial.print(type);
  Serial.print(F(": "));
  Serial.println(msg);
}

int getUserIdArg(const char *arg)
{
  if (!strcmp(arg, "0")) return 0;
  else if (!strcmp(arg, "1")) return 1;
  else if (!strcmp(arg, "2")) return 2;
  else return -1;
}

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
