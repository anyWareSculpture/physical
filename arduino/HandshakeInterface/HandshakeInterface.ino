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
#include "./Adafruit_NeoPixel.h"

#define TOUCH_SENSOR_PIN  2

// Which pin on the Arduino is connected to the NeoPixels?
#define NEOPIXEL_PIN 3
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(3, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Tuned for proximity sensor
#define TOUCH_THRESHOLD   50
#define	RELEASE_THRESHOLD 30

// I2C pins:
// SDA - A4
// SCL - A5

#include <Wire.h>
#include "./MPR121.h"

struct TouchSensor {
  int address;
  int irq_pin;
  bool status;

  TouchSensor(int pin, int addr = 0x5A) : irq_pin(pin), address(addr), status(false)  {}

  void setup() {
    Wire.begin();
    MPR121.begin(address);
    
    MPR121.setInterruptPin(irq_pin);

    // Enable _only_ electrode 0
    MPR121.setRegister(ECR, 0x81);
    
    MPR121.setTouchThreshold(TOUCH_THRESHOLD);
    MPR121.setReleaseThreshold(RELEASE_THRESHOLD);  
  }

  bool update() {
    bool changed = MPR121.touchStatusChanged();
    if (changed) {
      MPR121.updateTouchData();
      if (MPR121.isNewTouch(0)) status = true;
      else if (MPR121.isNewRelease(0)) status = false;
    }
    return changed;
  }

  bool getStatus() {
    return status;
  }

};

TouchSensor touch(TOUCH_SENSOR_PIN);


void setup()
{ 
  Serial.begin(115200);

  setupCommands();
  reset(DEBUG_MODE);
}

// Reset everything to initial state
void reset(bool debug)
{
  touch.setup();
  pixels.begin(); // This initializes the NeoPixel library.
  for (int i = 0; i < 3; i++) pixels.setPixelColor(i, BLACK);
  pixels.show();

  global_debug = debug;
  global_userid = -1;
  global_state = STATE_HANDSHAKE; // Handshake is always on
  Serial.println();
  Serial.println("HELLO handshake");
  if (global_debug) Serial.println("DEBUG handshake");
}

// Detect and send handshake
void handleHandshake()
{
  if (touch.update()) {
    bool active = touch.getStatus();
    Serial.print("HANDSHAKE ");
    Serial.println(active);

    if (active) do_handshake(active, global_userid);
  }
}

void do_handshake(bool active, uint8_t user)
{
  // Map user to LED ID and color
  uint8_t pixelid = user;

  if (active) {
    pixels.setPixelColor(pixelid, locationColor[user]);
    pixels.show();
  }
  else {
    if (user == global_userid) {
      // FIXME: Start pulsing the "attention LED"
    }
    else {
      pixels.setPixelColor(pixelid, BLACK);
      pixels.show();
    }
  }
}

void loop()
{
  handleSerial();
  handleHandshake();
}
