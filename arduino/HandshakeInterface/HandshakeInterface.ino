// Yes, this is actually -*-c++-*-

/*!
  This sketch is responsible for the Handshake interactions


  Example commands:
IDENTITY 0
HANDSHAKE 1 0
HANDSHAKE 1 1
HANDSHAKE 1 2
*/

#include "HandshakeInterface.h"
#include <Wire.h> // Needed to make Arduino add the corresponding include path
#include "TouchSensor.h"
#include "Pixel.h"
#include "LEDStrip.h"
#include "anyware_serial.h"

// Set to 1 to start in debug mode
#define DEBUG_MODE 1

#define TOUCH_SENSOR_PIN  2

// Tuned for proximity sensor
#define TOUCH_THRESHOLD   50
#define	RELEASE_THRESHOLD 30

// I2C pins:
// SDA - A4
// SCL - A5

TouchSensor touch(TOUCH_SENSOR_PIN);

// How many panels are attached to the Arduino?
#define STRIP_LEDS  3
CRGB leds[STRIP_LEDS];
Pixel pixels[STRIP_LEDS] = {
  Pixel(5, 0),
  Pixel(5, 1),
  Pixel(5, 2)
};
LEDStrip<SPI_DATA, SPI_CLOCK> strip(STRIP_LEDS, leds, pixels);

void setup()
{ 
  Serial.begin(115200);

  setupCommands();
  resetInterface(DEBUG_MODE);
}

void initInterface() {
}

// Reset everything to initial state
void resetInterface(bool debug)
{
  touch.setup();
  for (int i=0;i<LEDStripInterface::getNumStrips();i++) LEDStripInterface::getStrip(i).setup();
  LEDStripInterface::setAllColors(BLACK);

  global_debug = debug;
  global_state = STATE_HANDSHAKE; // Handshake is always on
  Serial.println();
  Serial.println("HELLO handshake");
  if (global_debug) Serial.println("DEBUG handshake");
  printCommands();
}

void do_panel_set(uint8_t strip, uint8_t panel, uint8_t intensity, const CRGB &color, AnywareEasing::EasingType  easing)
{
  CRGB newcol = applyIntensity(color, intensity);
  Serial.print(strip);Serial.print(" ");Serial.println(panel);
  const Pair &p = LEDStripInterface::mapToLED(strip, panel);
  if (p.stripid < 0 || p.pixelid < 0) {
    printError(F("client error"), F("Strip or panel out of range"));
    Serial.print(p.stripid);Serial.print(" ");Serial.println(p.pixelid);
    return;
  }
  LEDStripInterface &s = LEDStripInterface::getStrip(p.stripid);
  if (easing == AnywareEasing::BINARY) {
    s.setColor(p.pixelid, newcol);
    FastLED.show();
  }
  else {
    s.ease(p.pixelid, easing, newcol);
  }
}

void do_panel_pulse(uint8_t strip, uint8_t panel, uint8_t intensity, const CRGB &color, AnywareEasing::EasingType  easing)
{
  do_panel_set(strip, panel, intensity, color, easing);
}

void do_panel_intensity(uint8_t strip, uint8_t intensity)
{
  // FIXME: We don't currently support per-strip intensity, only global intensity
  FastLED.setBrightness(255*intensity/100);
  FastLED.show();
}

// Detect and send handshake
void handleHandshake()
{
  if (touch.update()) {
    bool active = touch.getStatus();
    Serial.print("HANDSHAKE ");
    Serial.println(active);
  }
}

uint32_t animPrevTickTime = 0;
void handleAnimations()
{
  uint32_t currtime = millis();

  if (currtime - animPrevTickTime >= 1) { // tick
    animPrevTickTime = currtime;
    LEDStripInterface::applyEasings();
  }
}

void loop()
{
  handleSerial();
  handleHandshake();
  handleAnimations();
}
