// Yes, this is actually -*-c++-*-

/*!
  This sketch is responsible for the Handshake interactions


  Example commands:
HANDSHAKE 1 0
HANDSHAKE 1 1
HANDSHAKE 1 2

PANEL-SET 5 0 100 user0 easein
PANEL-SET 5 1 100 user1 easein
PANEL-SET 5 2 100 user2 easein
//PANEL-SET 5 3 100 white easein
PANEL-PULSE 5 3 100 white sleep
PANEL-SET 6 0 100 user0
PANEL-SET 6 1 100 user0
PANEL-SET 6 2 100 user0

PANEL-SET 3 0 100 user0 easein
PANEL-SET 3 1 100 user1 easein
PANEL-SET 3 2 100 user2 easein
PANEL-SET 3 3 100 succedss easein
PANEL-SET 3 4 100 error easein
PANEL-SET 3 5 100 white easein

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
#define VIB_PIN  3
#define LED1_PIN  4
#define LED2_PIN  5
#define LED3_PIN  6

uint8_t highPowerLEDs[] = { LED1_PIN, LED2_PIN, LED3_PIN };
bool highPowerOnState[] = { false, false, true };

// Tuned for proximity sensor
#define TOUCH_THRESHOLD   50
#define	RELEASE_THRESHOLD 30

// I2C pins:
// SDA - A4
// SCL - A5

TouchSensor touch(TOUCH_SENSOR_PIN);

// How many panels are attached to the Arduino?
#define STRIP_LEDS  10
CRGB leds[STRIP_LEDS];
Pixel pixels[STRIP_LEDS] = {
  Pixel(5, 0),
  Pixel(5, 1),
  Pixel(5, 2),
  Pixel(5, 3),
  Pixel(3, 0),
  Pixel(3, 1),
  Pixel(3, 2),
  Pixel(3, 3),
  Pixel(3, 4),
  Pixel(3, 5),
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

  digitalWrite(VIB_PIN, 0);
  pinMode(VIB_PIN, OUTPUT);
  for (uint8_t i=0;i<3;i++) {
    digitalWrite(highPowerLEDs[i], !highPowerOnState[i]);
    pinMode(highPowerLEDs[i], OUTPUT);
  }


  global_debug = debug;
  global_state = STATE_HANDSHAKE; // Handshake is always on
  Serial.println();
  Serial.println("HELLO handshake");
  if (global_debug) Serial.println("DEBUG handshake");
  printCommands();
}

void do_panel_set(uint8_t strip, uint8_t panel, uint8_t intensity, const CRGB &color, AnywareEasing::EasingType easing, uint16_t duration)
{
  CRGB newcol = applyIntensity(color, intensity);

  if (strip == 6) {
    // Handle high-power LEDs separately
    uint8_t pin = highPowerLEDs[panel];
    digitalWrite(pin, intensity == 0 ? !highPowerOnState[panel] : highPowerOnState[panel]);
  return;
  }

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
    s.ease(p.pixelid, easing, newcol, duration);
  }
}

void do_panel_pulse(uint8_t strip, uint8_t panel, uint8_t intensity, const CRGB &color, AnywareEasing::EasingType  easing, uint16_t duration)
{
  do_panel_set(strip, panel, intensity, color, easing, duration);
}

void do_panel_intensity(uint8_t strip, uint8_t intensity)
{
  // FIXME: We don't currently support per-strip intensity, only global intensity
  FastLED.setBrightness(255*intensity/100);
  FastLED.show();
}

void do_handshake(uint8_t numusers)
{
  analogWrite(VIB_PIN, 255 * numusers / 3);
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
