// Yes, this is actually -*-c++-*-

/*!
  This sketch is responsible for the Disk game

  Example commands:

IDENTITY 0
DISK-INIT
DISK-RESET
DISK-STATE success
DISK 0 POS 10 DIR -1 USER 1
DISK 1 POS 50 DIR 1 USER 1
DISK 2 POS 60 DIR 1 USER 2
DISK 0 DIR 0 USER 1
DISK 0 DIR 1 USER 1
DISK 1 DIR -1 USER 1
DISK 2 DIR -1 USER 1
DISK-EXIT

PANEL-SET 3 0 100 user0
PANEL-SET 3 1 100 user1 easein
PANEL-SET 3 2 100 user2 easein
PANEL-SET 4 0 100 user0

*/

#include "./Timer.h"
#include "./Bounce2.h"
#include "DiskInterface.h"
#include "NeoPixel.h"
#include "anyware_colors.h"
#include "anyware_serial.h"

#if !defined(__AVR_ATmega1280__) && !defined(__AVR_ATmega2560__)
#error Oops!  Make sure you have 'Arduino Mega or Mega 2560' selected from the 'Tools -> Board' menu.
#endif

// Set to 1 to start in debug mode
#define DEBUG_MODE 1
// Define to start in auto-init mode (sets identity to 0 and state to Ready)
#define AUTOINIT

// Top disk
const int DISK0_LEFT_SENSOR = A0;
const int DISK0_RIGHT_SENSOR = A5;
const int DISK0_HOME_SENSOR = A10;
const int DISK0_COUNT_SENSOR = 38;
const int DISK0_RED_PIN = 2;
const int DISK0_GREEN_PIN = 3;
const int DISK0_BLUE_PIN = 4;
const int DISK0_MOTOR_A = 22;
const int DISK0_MOTOR_B = 24;

// Middle disk
const int DISK1_LEFT_SENSOR = A1;
const int DISK1_RIGHT_SENSOR = A4;
const int DISK1_HOME_SENSOR = A9;
const int DISK1_COUNT_SENSOR = 34;
const int DISK1_RED_PIN = 5;
const int DISK1_GREEN_PIN = 6;
const int DISK1_BLUE_PIN = 7;
const int DISK1_MOTOR_A = 26;
const int DISK1_MOTOR_B = 28;

// Bottom disk
const int DISK2_LEFT_SENSOR = A2;
const int DISK2_RIGHT_SENSOR = A3;
const int DISK2_HOME_SENSOR = A8;
const int DISK2_COUNT_SENSOR = 36;
const int DISK2_RED_PIN = 11;
const int DISK2_GREEN_PIN = 12;
const int DISK2_BLUE_PIN = 13;
const int DISK2_MOTOR_A = 30;
const int DISK2_MOTOR_B = 32;

// Globals

const int TEETH = 33;
const int REV = TEETH*2;

#define USER0_STR "0"
#define USER1_STR "1"
#define USER2_STR "2"

// Which pin on the Arduino is connected to the NeoPixels?
#define NEOPIXEL_PIN 52
// Right now, using a ring of 16 pixels
#define NUMNEOPIXELS 16
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMNEOPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Only 6 pixels used
NeoPixel neopixels[6] = {
  NeoPixel(0),
  NeoPixel(3),
  NeoPixel(5),
  NeoPixel(8),
  NeoPixel(10),
  NeoPixel(13)
};

Timer timer;
int8_t blinkerIdx = -1;

struct RgbLED {
  int redPin,greenPin,bluePin;
  uint32_t currColor;

  RgbLED(int r, int g, int b) :
    redPin(r), greenPin(g), bluePin(b) {}

  void setup() {
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);
    reset();
  }

  void reset() {
    setColor(BLACK);
  }
  
  uint32_t getCurrentColor() {
    return currColor;
  } 

  void setColor(uint32_t rgb) {
    currColor = rgb;
    uint8_t r = (uint8_t)(rgb >> 16);
    uint8_t g = (uint8_t)(rgb >>  8);
    uint8_t b = (uint8_t)rgb;
    analogWrite(redPin, 255-r);	 
    analogWrite(bluePin, 255-b);
    analogWrite(greenPin, 255-g);
  } 

};

struct Disk {
  int id;
  int leftSensor;
  int rightSensor;
  int homeSensor;
  int countSensor;
  int redPin,greenPin,bluePin;
  int motorA,motorB;

  Bounce leftIR;
  Bounce rightIR;
  Bounce countState;
  bool countChanged;

  int magnetValue;
  int lastMagnetValue;

  int remoteUser;
  Direction direction;

  int targetPosition;
  int actualPosition;

  static const int DISK_READY = 0;
  static const int DISK_HOMING = 1;
  int state;

  Disk(int diskid,
       int ls, int rs, int hs, int cs,
       int ma, int mb) :
    id(diskid), 
    leftSensor(ls), rightSensor(rs), homeSensor(hs), countSensor(cs),
    motorA(ma), motorB(mb),

    magnetValue(0), lastMagnetValue(0),
    actualPosition(0), direction(DIR_OFF),
    state(DISK_READY)
  {
    leftIR.attach(leftSensor);
    leftIR.interval(10);
    rightIR.attach(rightSensor);
    rightIR.interval(10);
    countState.attach(countSensor);
    countState.interval(50);
  }

  void setup() {
    pinMode(motorA, OUTPUT);
    pinMode(motorB, OUTPUT);
    pinMode(leftSensor,INPUT_PULLUP);
    pinMode(rightSensor,INPUT_PULLUP);
    pinMode(countSensor,INPUT_PULLUP);
    reset();
  }

  void reset() {
    remoteUser = -1;
    targetPosition = -1;
    direction = DIR_OFF;
  }
  
  void setState(int s) {
    state = s;
    Serial.print("DISK-STATE ");Serial.print(id);
    switch (state) {
    case DISK_READY:
      Serial.println(" ready");
      direction = DIR_OFF;
      break;
    case DISK_HOMING:
      Serial.println(" homing");
      direction = DIR_CCW;
      break;
    }
  }

  int getState() {
    return state;
  }

  /*!
    if pos is unspecified (-1), just move in the given direction
   */
  void setTargetPosition(int userid, int pos, Direction dir) {
    remoteUser = userid;
    targetPosition = pos;
    direction = dir;
  }

  int getPosition() {
    return actualPosition;
  }

  void readSensors() {
    leftIR.update();
    rightIR.update();
    countChanged = countState.update();
    magnetValue = analogRead(homeSensor);
  }

  void motorMove() {
    switch (direction) {
    case DIR_CW:
      digitalWrite(motorA, HIGH); 
      digitalWrite(motorB, LOW); //1A high and 2A low = turn right
      break;
    case DIR_CCW:
      digitalWrite(motorA, LOW); 
      digitalWrite(motorB, HIGH); //1A low and 2A high = turn left  
      break;
    default:
      digitalWrite(motorA, LOW); 
      digitalWrite(motorB, LOW); //1A low and 2A low = off
      break;
    }
  }

  bool magnetActive(int magnetValue) {
    return magnetValue > 220 || magnetValue < 200;
  }

  bool isHome() {
    return (magnetActive(magnetValue) && !magnetActive(lastMagnetValue));
  }

  void handleCount() {
    if (countChanged) {
      if (direction == DIR_CW) actualPosition = (actualPosition + REV - 1) % REV;
      else if (direction == DIR_CCW) actualPosition = (actualPosition + 1) % REV;

      if (global_state == STATE_READY) {
        Serial.print("DISK ");Serial.print(id);
        Serial.print(" POS ");Serial.print(actualPosition);
        Serial.print(" DIR ");Serial.println(direction);
      }
    }
  }

  void manage() {

    // Read local sensors
    readSensors();

    if (state == DISK_HOMING) {
      if (isHome()) {
        setState(DISK_READY);
        actualPosition = 0;
      }
    }
    else if (state == DISK_READY) {
      handleCount();

      if (targetPosition >= 0 && actualPosition == targetPosition) {
        direction = DIR_OFF;
        targetPosition = -1;
      }
    }
    motorMove();

    lastMagnetValue = magnetValue;
  }
};

Disk disk[3] = {
  Disk(0, DISK0_LEFT_SENSOR, DISK0_RIGHT_SENSOR, DISK0_HOME_SENSOR, DISK0_COUNT_SENSOR,
       DISK0_MOTOR_A, DISK0_MOTOR_B),
  Disk(1, DISK1_LEFT_SENSOR, DISK1_RIGHT_SENSOR, DISK1_HOME_SENSOR, DISK1_COUNT_SENSOR,
       DISK1_MOTOR_A, DISK1_MOTOR_B),
  Disk(2, DISK2_LEFT_SENSOR, DISK2_RIGHT_SENSOR, DISK2_HOME_SENSOR, DISK2_COUNT_SENSOR,
       DISK2_MOTOR_A, DISK2_MOTOR_B)
};

RgbLED rgbled[3] = {
  RgbLED(DISK0_RED_PIN, DISK0_GREEN_PIN, DISK0_BLUE_PIN),
  RgbLED(DISK1_RED_PIN, DISK1_GREEN_PIN, DISK1_BLUE_PIN),
  RgbLED(DISK2_RED_PIN, DISK2_GREEN_PIN, DISK2_BLUE_PIN)
};

void setGlobalState(int state) {
  global_state = state;
  if (global_state == STATE_HOMING) {
    blinkerIdx = timer.every(500, globalBlink, NULL);
    for (int i=0;i<3;i++) disk[i].setState(Disk::DISK_HOMING);
  }
}

void globalBlink(void *context) {
  if (global_state == STATE_HOMING) {
    uint32_t t = millis() / 100;
    uint32_t c = (t % 2) ? BLACK : RED;
    for (int i=0;i<3;i++) {
      if (disk[i].getState() == Disk::DISK_HOMING) rgbled[i].setColor(c);
      else rgbled[i].setColor(GREEN);
    }
  }
}

void manageDiskGame() {
  if (global_state == STATE_HOMING) {
    bool homed = true;
    for (int i=0;i<3;i++) {
      if (disk[i].getState() == Disk::DISK_HOMING) homed = false;
    }
    if (homed) {
      timer.stop(blinkerIdx);
      blinkerIdx = -1;
      for (int i=0;i<3;i++) rgbled[i].setColor(GREEN);
      delay(1000);
      setGlobalState(STATE_READY);
      for (int i=0;i<3;i++) rgbled[i].setColor(BLACK);
    }
  }

  for (int i=0;i<3;i++) disk[i].manage();
}

void setup()
{
  Serial.begin(115200);

  reset(DEBUG_MODE);
  setupCommands();
}

void setAllColors(uint32_t col) {
  for (int i = 0; i < NUMNEOPIXELS; i++) neopixels[i].setColor(col);
  pixels.show(); // This sends the updated pixel color to the hardware. 
}

// Reset everything to initial state
void reset(bool debug)
{
  setupIR();
  for (int i=0;i<3;i++) disk[i].setup();
  pixels.begin(); // This initializes the NeoPixel library.
  setAllColors(BLACK);
  setGlobalState(STATE_IDLE);

  global_debug = debug;
  global_userid = -1;
#ifdef AUTOINIT
  global_userid = 0;
  do_disk_init();
#endif

  Serial.println();
  Serial.println("HELLO disk");
  if (global_debug) Serial.println("DEBUG disk");
}

/*!
  DISK-INIT

  Initialize disk game. This just allows the Arduino to listen to DISK commands.
 */
void do_disk_init()
{
  setGlobalState(STATE_READY);
}

/*!
  DISK-EXIT

  Exit disk game. Stop any currently running animation or process.
  No longer listen to DISK commands.
*/
void do_disk_exit()
{
  setGlobalState(STATE_IDLE);

  timer.stop(blinkerIdx);
  for (int i=0;i<3;i++) disk[i].reset();
}

/*!
  DISK-RESET

  Resets the game, home the disks and afterwards to into "ready" mode.
*/
void do_disk_reset()
{
  timer.stop(blinkerIdx);
  for (int i=0;i<3;i++) disk[i].reset();
  setGlobalState(STATE_HOMING);
}

/*!
  pos can be unspecified (-1)
*/
void do_disk(uint8_t diskid, int userid, int pos, Direction dir)
{
  disk[diskid].setTargetPosition(userid, pos, dir);
}

void do_panel_set(uint8_t strip, uint8_t panel, uint8_t intensity, uint32_t color, AnywareEasing::EasingType  easing)
{
  color = applyIntensity(color, intensity);
  if (strip == 3) { // RGB LEDs
    rgbled[panel].setColor(color);
  }
  else if (strip == 4) { // NeoPixels
    uint8_t id = panel;
    if (easing == AnywareEasing::BINARY) {
      neopixels[id].setColor(color);
      pixels.show();
    }
    else {
      neopixels[id].ease(easing, color);
    }
  }
}

void do_panel_pulse(uint8_t strip, uint8_t panel, uint8_t intensity, uint32_t color, AnywareEasing::EasingType  easing)
{
  do_panel_set(strip, panel, intensity, color, easing);
}

// FIXME: We don't currently support per-strip intensity, only global intensity
void do_panel_intensity(uint8_t strip, uint8_t intensity)
{
  if (strip == 3) { // RGB LEDs
    // FIXME: Implement
  }
  else if (strip == 4) { // NeoPixels
    pixels.setBrightness(255*intensity/100);
    pixels.show();
  }
}

uint32_t animPrevTickTime = 0;
void handleAnimations()
{
  uint32_t currtime = millis();

  // FIXME: Handle RGB LEDs
  bool changed = false;
  if (currtime - animPrevTickTime >= 1) { // tick
    animPrevTickTime = currtime;
    for (uint8_t i=0;i<NUMNEOPIXELS;i++) {
      changed |= neopixels[i].applyEasing();
    }
  }
  if (changed) {
    //    Serial.println("show");
    pixels.show();
  }

}

void loop()
{
  timer.update();
  handleSerial();
  handleAnimations();

  if (global_state != STATE_IDLE) {
    manageDiskGame();
  }
}

