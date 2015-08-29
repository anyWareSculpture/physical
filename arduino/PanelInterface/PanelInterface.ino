// Yes, this is actually -*-c++-*-

/*!
  This sketch is responsible for the Panel Interface: IR sensors and RGB LEDs

  Example commands:

IDENTITY 0
INIT
PANEL-SET 0 0 100 user0 easein
PANEL-SET 0 1 100 user1 easein
PANEL-SET 0 2 100 user2 easein
PANEL-SET 0 3 100 success easein
PANEL-SET 0 4 100 white easein
PANEL-SET 0 5 100 user0 easein
PANEL-SET 0 6 100 user1 easein
PANEL-SET 0 7 100 user2 easein
PANEL-SET 0 8 100 success easein
PANEL-SET 0 9 100 white easein

PANEL-SET 0 0 100 user0
PANEL-SET 0 1 100 user1 easein
PANEL-SET 0 2 100 user2 easein
PANEL-SET 0 4 25 user0
PANEL-SET 0 5 100 user1
PANEL-SET 1 3 100 user2
PANEL-SET 1 4 100 error
PANEL-SET 1 5 100 success
PANEL-PULSE 0 3 100 success
PANEL-SET 2 6 100 black
PANEL-SET 2 6 100 user0 easein
PANEL-SET 2 6 100 user2 easein
PANEL-SET 2 6 100 success easein
PANEL-SET 2 6 100 black easein
PANEL-SET 2 6 100 user2 pop
PANEL-SET 2 6 100 user2 pulse
PANEL-SET 2 6 100 white pulse
PANEL-SET 2 6 100 white pop

PANEL-SET 0 3 100 user0 easein
PANEL-SET 0 4 100 user1 easein
PANEL-SET 1 3 100 user2 easein
PANEL-SET 2 3 100 white easein
PANEL-SET 2 6 100 success easein
PANEL-PULSE 2 6 100 success
PANEL-SET 2 6 100 black

PANEL-STATE success
PANEL-STATE failure
PANEL-EXIT


 */

#include "anyware_serial.h"
#include "anyware_colors.h"
#include "PanelInterface.h"
#include "./FastLED.h"
#include "FastPixel.h"
#include "SharpSensor.h"

// Set to 1 to start in debug mode
#define DEBUG_MODE 0
// Define to start in auto-init mode (sets identity to 0 and state to Ready)
#define AUTOINIT

// Sensitivity in millisecond. Less is more sensitive
#define IR_SENSITIVITY 50

// Define the array of leds
CRGB leds[NUMPANELS];

// Pins
const int IRPin1 = 2;  // IR pins
const int IRPin2 = 3;
const int IRPin3 = 4;
const int IRPin4 = 5;
const int IRPin5 = 6;
const int IRPin6 = 7;
const int IRPin7 = 8;
const int IRPin8 = 9;
const int IRPin9 = 10;
const int IRPin10 = 12;

// Configuration

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
  IRPixel(0, 9, 9, IRPin10),
  IRPixel(1, 0, -1, 14),
  IRPixel(1, 1, -1, 14),
  IRPixel(1, 2, -1, 14),
  IRPixel(1, 3, -1, 14),
  IRPixel(1, 4, -1, 14),
  IRPixel(1, 5, -1, 14),
  IRPixel(1, 6, -1, 14),
  IRPixel(1, 7, -1, 14),
  IRPixel(1, 8, -1, 14),
  IRPixel(1, 9, -1, 14),
  IRPixel(2, 0, -1, 14),
  IRPixel(2, 1, -1, 14),
  IRPixel(2, 2, -1, 14),
  IRPixel(2, 3, -1, 14),
  IRPixel(2, 4, -1, 14),
  IRPixel(2, 5, -1, 14),
  IRPixel(2, 6, -1, 14),
  IRPixel(2, 7, -1, 14),
  IRPixel(2, 8, -1, 14),
  IRPixel(2, 9, -1, 14)
};

void setAllColors(uint32_t col) {
  for (int i = 0; i < NUMPANELS; i++) irpixels[i].led.setColor(col);
  FastLED.show(); // This sends the updated pixel color to the hardware. 
}

void setup() {
  Serial.begin(115200);

  resetInterface(DEBUG_MODE);
  setupCommands();
}

void resetColors()
{
  setAllColors(BLACK);
}

void resetInterface(bool debug)
{
  for (int i=0;i<NUMPANELS;i++) irpixels[i].setup();
  FastLED.addLeds<APA102>(leds, NUMPANELS);
  resetColors();

  global_initialized = false;
  global_debug = debug;
  global_userid = -1;
  global_state = STATE_READY;

#ifdef AUTOINIT
  global_initialized = true;
  global_userid = 0;
#endif

  Serial.println();
  Serial.println(F("HELLO panel"));
  if (global_debug) Serial.println(F("DEBUG panel"));
}

void initInterface() {
}

void HandleSensors() {
  bool first = true;
  for (int i=0;i<NUMPANELS;i++) {
    if (irpixels[i].ir.readSensor()) {
      Serial.print(F("PANEL "));
      Serial.print(irpixels[i].strip);
      Serial.print(" ");
      Serial.print(irpixels[i].panel);
      Serial.print(" ");
      Serial.println(irpixels[i].ir.getState() ? 1 : 0);
    }
  }
}

void wrong() {
  setAllColors(RED);
  delay(1000);
  setAllColors(BLACK);
  resetColors();
}

void right() {
  for (int loopSuccess = 0; loopSuccess < 5; loopSuccess++) {
    setAllColors(GREEN);
    delay(200);
    setAllColors(BLACK);
    delay(200);     
  }
  resetColors();
}

// FIXME: We don't currently support duration
void do_panel_set(uint8_t strip, uint8_t panel, uint8_t intensity, uint32_t color, AnywareEasing::EasingType  easing)
{
  color = applyIntensity(color, intensity);
  uint8_t id = strip * 10 + panel;
  if (easing == AnywareEasing::BINARY) {
    irpixels[id].led.setColor(color);
    FastLED.show();
  }
  else {
    irpixels[id].led.ease(easing, color);
  }
}

// FIXME: We don't currently support duration
void do_panel_pulse(uint8_t strip, uint8_t panel, uint8_t intensity, uint32_t color, AnywareEasing::EasingType  easing)
{
  color = applyIntensity(color, intensity);
  uint8_t id = strip * 10 + panel;
  irpixels[id].led.ease(easing, color);
}

// FIXME: We don't currently support per-strip intensity, only global intensity
void do_panel_intensity(uint8_t strip, uint8_t intensity)
{
  FastLED.setBrightness(255*intensity/100);
  FastLED.show();
}

void do_panel_state(int state)
{
  global_state = state;
}

uint32_t animPrevTickTime = 0;
void handleAnimations()
{
  uint32_t currtime = millis();

  bool changed = false;
  if (currtime - animPrevTickTime >= 1) { // tick
    animPrevTickTime = currtime;
    for (uint8_t i=0;i<NUMPANELS;i++) {
      changed |= irpixels[i].led.applyEasing();
    }
  }
  if (changed) {
    //    Serial.println("show");
    FastLED.show();
  }

}

void loop()
{ 
  handleSerial();

  if (!global_initialized) return;
  
  handleAnimations();

  if (global_state == STATE_READY) {
    HandleSensors();
  }
  else if (global_state == STATE_SUCCESS) {
    right();
    global_state = STATE_READY;
    Serial.println(F("PANEL-STATE ready"));
  }
  else if (global_state == STATE_FAILURE) {
    wrong();
    global_state = STATE_READY;
    Serial.println(F("PANEL-STATE ready"));
  }
}
