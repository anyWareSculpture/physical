// Yes, this is actually -*-c++-*-

#include "./FastLED.h"
#include "SharpSensor.h"
#include "FastPixel.h"

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
// CLOCK PIN = 11
// DATA PIN = 13

// How many panels are attached to the Arduino?
#define NUMPANELS      10
CRGB leds[NUMPANELS];

struct IRPixel {
  uint8_t strip;
  uint8_t panel;
  FastPixel led;
  SharpSensor ir;

  IRPixel(uint8_t strip, uint8_t panel, int8_t pixelid, int8_t pin)
    : strip(strip), panel(panel), led(pixelid), ir(pin) {}

  void setup() {
    led.setup();
    ir.setup();
  }
};

IRPixel irpixels[NUMPANELS] = {
  IRPixel(0, 0, 0, IRPin1),
  IRPixel(0, 1, 1, IRPin2),
  IRPixel(0, 2, 2, IRPin3),
  IRPixel(0, 3, 3, IRPin4),
  IRPixel(0, 4, 4, IRPin5),
  IRPixel(0, 5, 5, IRPin6),
  IRPixel(0, 6, 6, IRPin7),
  IRPixel(0, 7, 7, IRPin8),
  IRPixel(0, 8, 8, IRPin9),
  IRPixel(0, 9, 9, IRPin10)
};

void setup() {
  Serial.begin(115200);
  Serial.println("Hello SimonTester");
  for (int i=0;i<NUMPANELS;i++) irpixels[i].setup();
  //  FastLED.addLeds<APA102>(leds, NUMPANELS);
  FastLED.addLeds<APA102, SPI_DATA, SPI_CLOCK, BGR, DATA_RATE_KHZ(100)>(leds, NUMPANELS);

  colorWipe(Color(255, 0, 0), 100); // Red
  colorWipe(Color(0, 255, 0), 100); // Green
  colorWipe(Color(0, 0, 255), 100); // Blue
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint8_t i=0; i<NUMPANELS; i++) {
    irpixels[i].led.setColor(c);
    FastLED.show();
    delay(wait);
  }
}

void loop() { 
  int numpixels = 0;
  for (int i=0;i<NUMPANELS;i++) {
    irpixels[i].ir.readSensor();
    if (irpixels[i].ir.getState()) numpixels++;
  }
  uint32_t col;
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
  for (int i=0;i<NUMPANELS;i++) irpixels[i].led.setColor(irpixels[i].ir.getState() ? col : BLACK);
  FastLED.show();
}
