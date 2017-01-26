// Yes, this is actually -*-c++-*-

#include "./FastLED.h"
#include "Sensor.h"
#include "Pixel.h"
#include "DiscreteSensors.h"
#include "LEDStrip.h"

// IR sensitivity - higher is less sensitive
#define SENSITIVITY 50

// Pins
const int IRPin1 = 2;  // IR pins
const int IRPin2 = 3;
const int IRPin3 = 4;
const int IRPin4 = 5;
const int IRPin5 = 6;
const int IRPin6 = 7;
const int IRPin7 = 8;
const int IRPin8 = 9;
const int IRPin9 = 14;
const int IRPin10 = 15;

// These are hardwired pins for the LED strip (SPI):
// DATA PIN = 11
// CLOCK PIN = 13

#define STRIP_LEDS 10
CRGB leds[STRIP_LEDS];
Pixel pixels[STRIP_LEDS] = {
  Pixel(0, 9),
  Pixel(0, 8),
  Pixel(0, 7),
  Pixel(0, 6),
  Pixel(0, 5),
  Pixel(0, 4),
  Pixel(0, 3),
  Pixel(0, 2),
  Pixel(0, 1),
  Pixel(0, 0)
};
LEDStrip<SPI_DATA, SPI_CLOCK> strip(STRIP_LEDS, leds, pixels);

#define STRIP_SENSORS 10
Sensor strip_sensors[STRIP_SENSORS] = {
  Sensor(0, 9, IRPin1),
  Sensor(0, 8, IRPin2),
  Sensor(0, 7, IRPin3),
  Sensor(0, 6, IRPin4),
  Sensor(0, 5, IRPin5),
  Sensor(0, 4, IRPin6),
  Sensor(0, 3, IRPin7),
  Sensor(0, 2, IRPin8),
  Sensor(0, 1, IRPin9),
  Sensor(0, 0, IRPin10)
};
DiscreteSensors sensors(STRIP_SENSORS, strip_sensors);

void setup() {
  Serial.begin(115200);
  Serial.println("Hello SimonTester");
  for (int i=0;i<LEDStripInterface::getNumStrips();i++) LEDStripInterface::getStrip(i).setup();
  sensors.setup();

  colorWipe(CRGB(255, 0, 0), 100); // Red
  colorWipe(CRGB(0, 255, 0), 100); // Green
  colorWipe(CRGB(0, 0, 255), 100); // Blue
  colorWipe(CRGB(0, 0, 0), 100); // BLACK
}

// Fill the dots one after the other with a color
void colorWipe(const CRGB &c, uint8_t wait) {
  for (int i=0;i<LEDStripInterface::getNumStrips();i++) {
    LEDStripInterface &s = LEDStripInterface::getStrip(i);
    for(uint8_t i=0; i<s.getNumPixels(); i++) {
      s.setColor(i, c);
      FastLED.show();
      delay(wait);
    }
  }
}

void loop() { 
  int numpixels = 0;
  for (int i=0;i<sensors.getNumSensors();i++) {
    Sensor &s = sensors.getSensor(i);
    s.readSensor();
    if (s.getState()) {
      Serial.print(s.panel);
      Serial.print(" ");
      numpixels++;
    }
  }
  if (numpixels > 0) Serial.println();
  CRGB col;
  switch (numpixels) {
  case 0:
    col = BLACK;
    break;
  case 1:
    col = RED;
    break;
  case 2:
    col = GREEN;
    break;
  case 3:
    col = MYBLUE;
    break;
  default:
    col = MYPINK;
    break;
  }
  LEDStripInterface &s = LEDStripInterface::getStrip(0);
  for (int i=0;i<s.getNumPixels();i++) s.setColor(i, sensors.getSensor(i).getState() ? col : BLACK);
  FastLED.show();
}
