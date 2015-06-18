// Yes, this is actually -*-c++-*-

/*!
  This sketch is responsible for the Panel Interface

  Example commands:

IDENTITY 0
PANEL-INIT
PANEL-SET 0 3 100 user0
PANEL-SET 0 4 25 user0
PANEL-SET 0 5 100 user1
PANEL-SET 1 3 100 user2
PANEL-SET 1 4 100 error
PANEL-SET 1 5 100 success
PANEL-PULSE 0 3 100 error
PANEL-STATE success
PANEL-STATE failure
PANEL-EXIT

 */

#include "./Adafruit_NeoPixel.h"
#include "./Bounce2.h"
#include "anyware_global.h"
#include "PanelInterface.h"


// Pins
const int IRPin1 = 4;  // IR pins
const int IRPin2 = 5;
const int IRPin3 = 6;
const int IRPin4 = 7;
const int IRPin5 = 8;
const int IRPin6 = 9;
const int IRPin7 = 10;
const int IRPin8 = 11;
const int IRPin9 = 12;
const int IRPin10 = 13;

// Sensitivity in millisecond. Less is more sensitive
#define IR_SENSITIVITY 50

// Which pin on the Arduino is connected to the NeoPixels?
#define NEOPIXEL_PIN 2
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPANELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

int currSensor = -1;   // Storage of which IR is pressed
int prevSensor = -1;    // Storage of which IR is pressed
unsigned long lastActionTime;

// Configuration

struct IRPixel {
  uint8_t strip;
  uint8_t panel;
  int8_t pixel;
  int8_t sensorPin;

  Bounce irState;
  bool state;

  IRPixel(uint8_t strip, uint8_t panel, int8_t pixelid, int8_t pin) : strip(strip), panel(panel), pixel(pixelid), sensorPin(pin), state(false) {}

  void setup() {
    if (sensorPin >= 0) {
      pinMode(sensorPin, INPUT_PULLUP);
      irState.attach(sensorPin);
      irState.interval(IR_SENSITIVITY);
    }
  }
  
  // Returns true if sensor state changed
  bool readSensor() {
    if (irState.update()) {
      state = !irState.read();
      return true;
    }
    return false;
  }

  bool getState() {
    return state;
  }

  void setColor(uint32_t col) {
    if (pixel >= 0) pixels.setPixelColor(pixel, col);
  }

  uint32_t getColor() {
    return pixel >= 0 ? pixels.getPixelColor(pixel) : 0;
  }

};

IRPixel irpixels[NUMPANELS] = {
  IRPixel(0, 0, -1, 14),
  IRPixel(0, 1, -1, 14),
  IRPixel(0, 2, -1, 14),
  IRPixel(0, 3, 0, IRPin1),
  IRPixel(0, 4, 1, IRPin2),
  IRPixel(0, 5, 2, IRPin3),
  IRPixel(0, 6, -1, 14),
  IRPixel(0, 7, -1, 14),
  IRPixel(0, 8, -1, 14),
  IRPixel(0, 9, -1, 14),
  IRPixel(1, 0, -1, 14),
  IRPixel(1, 1, -1, 14),
  IRPixel(1, 2, -1, 14),
  IRPixel(1, 3, 3, IRPin4),
  IRPixel(1, 4, 4, IRPin5),
  IRPixel(1, 5, 5, IRPin6),
  IRPixel(1, 6, -1, 14),
  IRPixel(1, 7, -1, 14),
  IRPixel(1, 8, -1, 14),
  IRPixel(1, 9, -1, 14),
  IRPixel(2, 0, -1, 14),
  IRPixel(2, 1, -1, 14),
  IRPixel(2, 2, -1, 14),
  IRPixel(2, 3, 6, IRPin7),
  IRPixel(2, 4, 7, IRPin8),
  IRPixel(2, 5, 8, IRPin9),
  IRPixel(2, 6, 9, IRPin10),
  IRPixel(2, 7, -1, 14),
  IRPixel(2, 8, -1, 14),
  IRPixel(2, 9, -1, 14)
};

void setAllColors(uint32_t col) {
  for (int i = 0; i < NUMPANELS; i++) irpixels[i].setColor(col);
  pixels.show(); // This sends the updated pixel color to the hardware. 
}

void setup() {
  Serial.begin(115200);

  setupCommands();
  reset(DEBUG_MODE);
}

void loop()
{ 
  handleSerial();

  if (global_state == STATE_READY) {
    HandleSensors();
  }
  else if (global_state == STATE_SUCCESS) {
    right();
    global_state = STATE_READY;
    Serial.println("PANEL-STATE ready");
  }
  else if (global_state == STATE_FAILURE) {
    wrong();
    global_state = STATE_READY;
    Serial.println("PANEL-STATE ready");
  }
}

void resetColors()
{
  setAllColors(BLACK);
}

void reset(bool debug)
{
  setupIR();
  for (int i=0;i<NUMPANELS;i++) irpixels[i].setup();
  pixels.begin(); // This initializes the NeoPixel library.
  resetColors();

  global_debug = debug;
  global_userid = -1;
  global_state = STATE_IDLE;

  Serial.println();
  Serial.println("HELLO panel");
  if (global_debug) Serial.println("DEBUG panel");
}

void HandleSensors() {
  bool first = true;
  for (int i=0;i<NUMPANELS;i++) {
    if (irpixels[i].readSensor()) {
      Serial.print("PANEL ");
      Serial.print(irpixels[i].strip);
      Serial.print(" ");
      Serial.print(irpixels[i].panel);
      Serial.print(" ");
      Serial.println(irpixels[i].getState() ? 1 : 0);
    }
  }
}

void wrong() {
  setAllColors(BLACK);

  if (currSensor >= 0) {
    irpixels[currSensor].setColor(RED);
    pixels.show();
  }

  delay(1000);
  setAllColors(BLACK);
  resetColors();
  lastActionTime = 0;
}

void right() {
  for (int loopSuccess = 0; loopSuccess < 5; loopSuccess++) {
    setAllColors(GREEN);
    delay(200);
    setAllColors(BLACK);
    delay(200);     
  }
  resetColors();
  lastActionTime = 0;
}

// Returns the index of the first active sensor or -1 if no sensors are active
int getCurrentSensor() {
  for (int i=0;i<NUMPANELS;i++) {
    if (irpixels[i].getState()) return i;
  }
  return -1; 
}

// Returns false if more than one sensor is active
uint8_t numSensorsActive() {
  int count = 0;
  for (int i=0;i<NUMPANELS;i++) {
    if (irpixels[i].getState()) {
      count++;
    }
  }
  return count;
}

void do_panel_init()
{
  global_state = STATE_READY;
}

void do_panel_exit()
{
  global_state = STATE_IDLE;
}

void do_panel_set(uint8_t strip, uint8_t panel, uint8_t intensity, uint32_t color)
{
  uint8_t id = strip * 10 + panel;
  irpixels[id].setColor(color);
  pixels.show();
}

void do_panel_pulse(uint8_t strip, uint8_t panel, uint8_t intensity, uint32_t color)
{
  uint8_t id = strip * 10 + panel;
  uint32_t oldcolor = irpixels[id].getColor();
  irpixels[id].setColor(color);
  pixels.show();
  delay(500);
  irpixels[id].setColor(oldcolor);
  pixels.show();
}

void do_panel_intensity(uint8_t strip, uint8_t intensity)
{
  pixels.setBrightness(255*intensity/100);
  pixels.show();
}

void do_panel_state(int state)
{
  global_state = state;
}
