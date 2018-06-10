// Yes, this is actually -*-c++-*-

/*!
  This sketch is responsible for the Panel Interface: IR sensors and RGB LEDs

  Example commands:

IDENTITY 0
INIT
PANEL-SET 1 0 100 sculpture1 easein
PANEL-SET 1 1 100 sculpture2 easein
PANEL-SET 3 2 100 sculpture3 easein
PANEL-SET 0 3 100 success easein
PANEL-SET 0 4 100 white easein
PANEL-SET 0 5 100 sculpture1 easein
PANEL-SET 0 6 100 sculpture2 easein
PANEL-SET 0 7 100 sculpture3 easein
PANEL-SET 0 8 100 success easein
PANEL-SET 0 9 100 white easein

PANEL-SET 0 012345 100 sculpture1
PANEL-SET 0 6789 100 sculpture3

PANEL-SET 0 0 100 sculpture1
PANEL-SET 0 1 100 sculpture2 easein
PANEL-SET 0 2 100 sculpture3 easein
PANEL-SET 0 4 25 sculpture1
PANEL-SET 0 5 100 sculpture2
PANEL-SET 1 3 100 sculpture3
PANEL-SET 1 4 100 error
PANEL-SET 1 5 100 success
PANEL-PULSE 0 3 100 success
PANEL-SET 2 6 100 black
PANEL-SET 2 6 100 sculpture1 easein
PANEL-SET 2 6 100 sculpture3 easein
PANEL-SET 2 6 100 success easein
PANEL-SET 2 6 100 black easein
PANEL-SET 2 6 100 sculpture3 pop
PANEL-SET 2 6 100 sculpture3 pulse
PANEL-SET 2 6 100 white pulse
PANEL-SET 2 6 100 white pop

PANEL-SET 0 3 100 sculpture1 easein
PANEL-SET 0 4 100 sculpture2 easein
PANEL-SET 1 3 100 sculpture3 easein
PANEL-SET 2 3 100 white easein
PANEL-SET 2 6 100 success easein
PANEL-PULSE 2 6 100 success
PANEL-SET 2 6 100 black

PANEL-EXIT

PANEL-SET 2 3 100 white
PANEL-SET 2 3 0 white


 */

#include "anyware_serial.h"
#include "anyware_colors.h"
#include "PanelInterface.h"
#include "./FastLED.h"
#include "Sensor.h"
#include "Pixel.h"
#include "DiscreteSensors.h"
#include "LEDStrip.h"

// Set to 1 to start in debug mode
#define DEBUG_MODE 0

// Sensitivity in millisecond. Less is more sensitive
#define IR_SENSITIVITY 50

// Configuration

#define ANYWARE_MAIN
#include "configuration.h"
#include "Controllers.h"

void setup() {
  Serial.begin(115200);

  resetInterface(DEBUG_MODE);
  setupCommands();
}

void resetColors()
{
  LEDStripInterface::setAllColors(BLACK);
}

void resetInterface(bool debug)
{
  for (int i=0;i<LEDStripInterface::getNumStrips();i++) LEDStripInterface::getStrip(i).setup();
  sensors.setup();
  resetColors();

  global_debug = debug;

  Serial.println();
  Serial.println(F("HELLO panel V1.3"));
  if (global_debug) Serial.println(F("DEBUG panel"));
  printCommands();
}

void initInterface() {
}

void handleSensors() {
  for (int i=0;i<sensors.getNumSensors();i++) {
    const Sensor &s = sensors.getSensor(i);
    if (sensors.readSensor(i)) {
      Serial.print(F("PANEL "));
      Serial.print(s.strip);
      Serial.print(" ");
      Serial.print(s.panel);
      Serial.print(" ");
      Serial.println(sensors.getState(i) ? 1 : 0);
    }
  }
}

// FIXME: We don't currently support duration
void do_panel_set(uint8_t strip, PanelSet panels, uint8_t intensity, const CRGB &color, AnywareEasing::EasingType  easing)
{
  CRGB newcol = applyIntensity(color, intensity);
  
  for (uint8_t i=0;i<STRIP_LEDS;++i) {
    if (panels.get(i)) {
      const uint8_t panel = i;
      const Pair &p = LEDStripInterface::mapToLED(strip, panel);
      if (p.stripid < 0 || p.pixelid < 0) {
	printError(F("client error"), F("Strip or panel out of range"));
	return;
      }
      LEDStripInterface &s = LEDStripInterface::getStrip(p.stripid);
      if (easing == AnywareEasing::BINARY) {
	s.setColor(p.pixelid, newcol);
      }
      else {
	// FIXME: We don't yet support easing of groups of pixels
	s.ease(p.pixelid, easing, newcol);
      }
    }
  }
  if (easing == AnywareEasing::BINARY) {
    FastLED.show();
  }
}

// FIXME: We don't currently support duration
void do_panel_pulse(uint8_t strip, PanelSet panels, uint8_t intensity, const CRGB &color, AnywareEasing::EasingType  easing)
{
  for (uint8_t panel=0;panel<STRIP_LEDS;++panel) {
    if (panels.get(panel)) {
      CRGB newcol = applyIntensity(color, intensity);
      const Pair &p = LEDStripInterface::mapToLED(strip, panel);
      if (p.stripid < 0 || p.pixelid < 0) {
	printError(F("client error"), F("Strip or panel out of range"));
	return;
      }
      LEDStripInterface &s = LEDStripInterface::getStrip(p.stripid);
      s.ease(p.pixelid, easing, newcol);
    }
  }
}

// FIXME: We don't currently support per-strip intensity, only global intensity
void do_panel_intensity(uint8_t strip, uint8_t intensity)
{
  FastLED.setBrightness(255*intensity/100);
  FastLED.show();
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

  handleAnimations();

  handleSensors();
}
