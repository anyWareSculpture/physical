// Yes, this is actually -*-c++-*-

// These are hardwired pins for the LED strip (SPI):
// DATA PIN = 11
// CLOCK PIN = 13

// I2C pins:
// SDA - A4
// SCL - A5

#define TOUCH_SENSOR_PIN  2
#define VIB_PIN  3

#define LED1_PIN  4
#define LED2_PIN  5
#define LED3_PIN  6

#include <Wire.h> // Needed to make Arduino add the corresponding include path
#include "TouchSensor.h"
#include "Pixel.h"
#include "LEDStrip.h"
#include "anyware_colors.h"

TouchSensor touch(TOUCH_SENSOR_PIN);

// How many panels are attached to the Arduino?
#define STRIP_LEDS  3
CRGB leds[STRIP_LEDS];
Pixel pixels[STRIP_LEDS] = {
  Pixel(0, 0),
  Pixel(0, 1),
  Pixel(0, 2)
};
LEDStrip<SPI_DATA, SPI_CLOCK> strip(STRIP_LEDS, leds, pixels);

void setup() {
  Serial.begin(115200);
  Serial.println("Hello HandshakeTester");

  for (int i=0;i<LEDStripInterface::getNumStrips();i++) LEDStripInterface::getStrip(i).setup();

  touch.setup();

  pinMode(VIB_PIN, OUTPUT);
  digitalWrite(LED1_PIN, 1);
  digitalWrite(LED2_PIN, 1);
  digitalWrite(LED3_PIN, 1);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);

  colorWipe(RED, 1000); // Red
  colorWipe(GREEN, 1000); // Green
  colorWipe(BLUE, 1000); // Blue
  colorWipe(BLACK, 1000); // Blue
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (int i=0;i<LEDStripInterface::getNumStrips();i++) {
    LEDStripInterface &s = LEDStripInterface::getStrip(i);
    for(uint8_t i=0; i<s.getNumPixels(); i++) {
      s.setColor(i, c);
      FastLED.show();
      delay(wait);
    }
  }
}

// Detect and send handshake
void handleHandshake()
{
  if (touch.update()) {
    bool active = touch.getStatus();
    Serial.print("HANDSHAKE ");
    Serial.println(active);

    do_handshake(active);
  }
}

void do_handshake(bool active)
{
  if (active) {
    strip.ease(0, AnywareEasing::IN_CUBIC, RED);
    strip.ease(1, AnywareEasing::IN_CUBIC, GREEN);
    strip.ease(2, AnywareEasing::IN_CUBIC, BLUE);
  }
  else {
    strip.ease(0, AnywareEasing::IN_CUBIC, BLACK);
    strip.ease(1, AnywareEasing::IN_CUBIC, BLACK);
    strip.ease(2, AnywareEasing::IN_CUBIC, BLACK);
  }
  FastLED.show();

  digitalWrite(VIB_PIN, active);
  digitalWrite(LED1_PIN, !active);
  digitalWrite(LED2_PIN, !active);
  digitalWrite(LED3_PIN, !active);
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

void loop() { 
  handleHandshake();
  handleAnimations();
}
