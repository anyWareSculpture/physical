// Yes, this is actually -*-c++-*-

/*!
  This sketch is responsible for the Simon game

  Example commands:

IDENTITY 0
SIMON-INIT
SIMON-PATTERN 0 2 8
SIMON-SENSORS 2 1 0 0 1 1 0 0 0 1 0
SIMON-SENSORS 2 0 0 0 0 0 0 0 0 0 0
SIMON-STATE success
SIMON-STATE failure
SIMON-EXIT

 */

#include "./Adafruit_NeoPixel.h"
#include "./Bounce2.h"
#include "anyware_global.h"
#include "SimonGame.h"

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
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
bool remoteSensors[NUMPIXELS];

int currSensor = -1;   // Storage of which IR is pressed
int prevSensor = -1;    // Storage of which IR is pressed
unsigned long lastActionTime;


struct Combination {
  uint8_t length;
  uint8_t combo[MAX_SIMON_PATTERN];
  Combination() : length(0) {}

  void setCombination(uint8_t *pattern, uint8_t count) {
    for (uint8_t i=0;i<count;i++) combo[i] = pattern[i];
    length = count;
  }

  uint8_t getLength() { return length; }

  int getPixel(uint8_t idx) { return combo[idx]; }
};

// Configuration

unsigned long wait = 3000;   // This is how long Simon waits before restarting
int dictateDelay = 800;   // How fast is the dictate being played back

Combination currentPattern;

struct IRPixel {
  int id;
  int sensorPin;

  Bounce irState;
  bool state;

  IRPixel(int pixelid, int pin) : id(pixelid), sensorPin(pin), state(false) {}

  void setup() {
    pinMode(sensorPin, INPUT_PULLUP);
    irState.attach(sensorPin);
    irState.interval(IR_SENSITIVITY);
  }
  
  // Returns true if sensor state changed
  bool readSensor() {
    if (irState.update()) {
      state = irState.read();
      return true;
    }
    return false;
  }

  bool getState() {
    return state;
  }

  void setColor(uint32_t col) {
    pixels.setPixelColor(id, col);
  }

};

IRPixel irpixels[NUMPIXELS] = {
  IRPixel(0, IRPin1),
  IRPixel(1, IRPin2),
  IRPixel(2, IRPin3),
  IRPixel(3, IRPin4),
  IRPixel(4, IRPin5),
  IRPixel(5, IRPin6),
  IRPixel(6, IRPin7),
  IRPixel(7, IRPin8),
  IRPixel(8, IRPin9),
  IRPixel(9, IRPin10)
};

void setAllColors(uint32_t col) {
  for (int i = 0; i < NUMPIXELS; i++) irpixels[i].setColor(col);
  pixels.show(); // This sends the updated pixel color to the hardware. 
}

void setComboColors(uint32_t col) {
  for (int i = 0; i < currentPattern.getLength(); i++) {
    irpixels[currentPattern.getPixel(i)].setColor(col);
  }
  pixels.show();
}


void setup() {
  Serial.begin(115200);

  setupCommands();
  reset(DEBUG_MODE);
}

void loop()
{ 
  handleSerial();

  // FIXME: Implement playback asynchronously
  if (global_state == STATE_PLAYBACK) {
    SimonDictates();
    resetColors();
    global_state = STATE_READY;
    Serial.println("SIMON-STATE ready");
    lastActionTime = millis();
  }
  else if (global_state == STATE_READY) {
    HandleSensors();
  }
  else if (global_state == STATE_SUCCESS) {
    right();
    global_state = STATE_READY;
    Serial.println("SIMON-STATE ready");
  }
  else if (global_state == STATE_FAILURE) {
    wrong();
    global_state = STATE_READY;
    Serial.println("SIMON-STATE ready");
  }
}

void SimonDictates() { // here the program runs through a combination in the array NeoPixel
  // loop from the lowest neopixel to the highest:
  for (int i = 0; i < currentPattern.getLength(); i++) {
    irpixels[currentPattern.getPixel(i)].setColor(WHITE); // pink color.
    if (i > 0) irpixels[currentPattern.getPixel(i-1)].setColor(BLACK); // pink color.
    pixels.show(); // This sends the updated pixel color to the hardware.
    delay(dictateDelay);
  }
  irpixels[currentPattern.getPixel(currentPattern.getLength()-1)].setColor(BLACK); // no color.
  pixels.show(); // This sends the updated pixel color to the hardware.
}

void resetColors()
{
  setAllColors(BLACK);
  for (uint8_t i=0;i<NUMPIXELS;i++) remoteSensors[i] = false;
}

void reset(bool debug)
{
  setupIR();
  for (int i=0;i<NUMPIXELS;i++) irpixels[i].setup();
  pixels.begin(); // This initializes the NeoPixel library.
  resetColors();

  global_debug = debug;
  global_userid = -1;
  global_state = STATE_IDLE;

  Serial.println();
  Serial.println("HELLO simon");
  if (global_debug) Serial.println("DEBUG simon");
}

void HandleSensors() {
  bool changed = false;
  for (int i=0;i<NUMPIXELS;i++) {
    if (irpixels[i].readSensor()) {
      changed = true;
      irpixels[i].setColor(irpixels[i].getState() ? locationColor[global_userid] : BLACK);
    }
  }

  if (changed) {
    pixels.show();
    Serial.print("SIMON-SENSORS");
    for (int i=0;i<NUMPIXELS;i++) {
      Serial.print(" ");
      Serial.print(irpixels[i].getState() ? 1 : 0);
    }
    Serial.println();
  }
}

void wrong() {
  setAllColors(BLACK);

  if (currSensor >= 0) {
    irpixels[currSensor].setColor(RED);
    pixels.show();
  }
  //  setComboColors(RED);

  delay(1000);
  setAllColors(BLACK);
  resetColors();
  lastActionTime = 0;
}

void right() {
  for (int loopSuccess = 0; loopSuccess < 5; loopSuccess++) {
    setComboColors(GREEN);
    delay(200);
    setComboColors(BLACK);
    delay(200);     
  }
  resetColors();
  lastActionTime = 0;
}

// Returns the index of the first active sensor or -1 if no sensors are active
int getCurrentSensor() {
  for (int i=0;i<NUMPIXELS;i++) {
    if (irpixels[i].getState()) return i;
  }
  return -1; 
}

// Returns false if more than one sensor is active
uint8_t numSensorsActive() {
  int count = 0;
  for (int i=0;i<NUMPIXELS;i++) {
    if (irpixels[i].getState()) {
      count++;
    }
  }
  return count;
}

void do_simon_init()
{
  global_state = STATE_READY;
}

void do_simon_exit()
{
  global_state = STATE_IDLE;
}

void do_simon_pattern(uint8_t *pattern, uint8_t count)
{
  currentPattern.setCombination(pattern, count);
  global_state = STATE_PLAYBACK;
  Serial.println("SIMON-STATE playback");
}

void do_simon_sensors(uint8_t userid)
{
  for (uint8_t i=0;i<NUMPIXELS;i++) {
    irpixels[i].setColor(remoteSensors[i] ? locationColor[userid] : BLACK);
    // FIXME: How to merge with local state?
  }
  pixels.show();
}

void do_simon_state(int state)
{
  global_state = state;
}
