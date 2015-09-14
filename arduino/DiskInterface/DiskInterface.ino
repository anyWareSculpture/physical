// Yes, this is actually -*-c++-*-

/*!
  This sketch is responsible for the Disk game

  Example commands:

IDENTITY 0
DISK-INIT
DISK-RESET
DISK-STATE success
DISK 0 POS 10 DIR 1 USER 1
DISK 1 POS 50 DIR 1 USER 1
DISK 2 POS 60 DIR 1 USER 2
DISK 0 DIR 0 USER 1
DISK 0 DIR 1 USER 1
DISK 0 DIR -1 USER 1
DISK 0 DIR 0 USER 1
DISK 1 DIR 0 USER 1
DISK 2 DIR 0 USER 1
DISK-EXIT

PANEL-SET 4 0 100 user0 easein
PANEL-SET 4 0 100 white pulse
PANEL-SET 4 0 100 white pop
PANEL-SET 4 1 100 user1 easein
PANEL-SET 4 2 100 user2 easein
PANEL-SET 3 0 100 error easein
PANEL-SET 3 0 100 white pulse

*/

#if !defined(__MK20DX256__)
#error Oops!  Make sure you have 'Teensy 3.1' selected from the 'Tools -> Board' menu.
#endif

#include "./Timer.h"
#include "./Bounce2.h"
#include "DiskInterface.h"
#include "anyware_colors.h"
#include "anyware_serial.h"
#include "LEDStrip.h"

// Set to 1 to start in debug mode
#define DEBUG_MODE 1
// Define to start in auto-init mode (sets identity to 0 and state to Ready)
#define AUTOINIT

// Top disk
const int DISK0_HOME_SENSOR = A0;
const int DISK0_ENC_A = 5;
const int DISK0_ENC_B = 6;
const int DISK0_MOTOR_A = 4;
const int DISK0_MOTOR_B = 3;

// Middle disk
const int DISK1_HOME_SENSOR = A1;
const int DISK1_ENC_A = 9;
const int DISK1_ENC_B = 10;
const int DISK1_MOTOR_A = 8;
const int DISK1_MOTOR_B = 7;

// Bottom disk
const int DISK2_HOME_SENSOR = A2;
const int DISK2_ENC_A = 20;
const int DISK2_ENC_B = 19;
const int DISK2_MOTOR_A = 17;
const int DISK2_MOTOR_B = 18;

// Globals

const int TICKS_PER_REVOLUTION = 31000;
const float TICKS_PER_DEGREE = 1.0f*TICKS_PER_REVOLUTION/360;

const uint8_t MAGNET_SENSITIVITY = 140;

#define USER0_STR "0"
#define USER1_STR "1"
#define USER2_STR "2"

#define STRIP_LEDS 9
CRGB strip_leds[STRIP_LEDS];
Pixel strip_pixels[STRIP_LEDS] = {
  Pixel(4, 0),
  Pixel(4, 1),
  Pixel(4, 2),
  Pixel(3, 0),
  Pixel(3, 1),
  Pixel(3, 2),
  Pixel(3, 3),
  Pixel(3, 4),
  Pixel(3, 5),
};
LEDStrip<SPI_DATA, SPI_CLOCK> strip(STRIP_LEDS, strip_leds, strip_pixels);

Timer timer;
int8_t blinkerIdx = -1;
int8_t homingIdx = -1;

struct Disk {
  int id;
  int homeSensor;
  int enca, encb;
  int motorA,motorB;

  void (*enca_isr)();
  void (*encb_isr)();

  bool enca_val;
  bool encb_val;

  int magnetValue;
  int lastMagnetValue;

  uint16_t targetPosition;
  uint16_t actualPosition;
  int32_t tickCount;
  bool posChanged;

  Direction direction;

  static const int DISK_READY = 0;
  static const int DISK_HOMING = 1;
  int state;

  int32_t lastExitTicks;
  int32_t lastHitTicks;

  Disk(int diskid, int hs, int enca, int encb, int ma, int mb, void (*enca_isr)(), void (*encb_isr)()) :
    id(diskid), homeSensor(hs), enca(enca), encb(encb), motorA(ma), motorB(mb),
    enca_isr(enca_isr), encb_isr(encb_isr),
    enca_val(false), encb_val(false),
    magnetValue(0), lastMagnetValue(0),
    actualPosition(0), tickCount(0), posChanged(false),
    direction(DIR_OFF), state(DISK_READY),
    lastExitTicks(0), lastHitTicks(0)
  {
  }

  void setup() {
    pinMode(motorA, OUTPUT);
    pinMode(motorB, OUTPUT);
    pinMode(enca, INPUT_PULLUP);
    pinMode(encb, INPUT_PULLUP);
    enca_val = digitalRead(enca);
    encb_val = digitalRead(encb);
    reset();
    attachInterrupt(enca, enca_isr, CHANGE);
    attachInterrupt(encb, encb_isr, CHANGE);
  }

  void reset() {
    targetPosition = -1;
    direction = DIR_OFF;
  }

  inline uint16_t ticksToPos(int32_t ticks) {
    if (ticks < 0) ticks += TICKS_PER_REVOLUTION;
    return (ticks * 360 / TICKS_PER_REVOLUTION) % 360;
  }
  
  inline void handleEncA() {
    enca_val = digitalRead(enca);
    if (enca_val != encb_val) tickCount = (tickCount-1+TICKS_PER_REVOLUTION)%TICKS_PER_REVOLUTION;
    else tickCount = (tickCount+1)%TICKS_PER_REVOLUTION;
  }

  inline void handleEncB() {
    encb_val = digitalRead(encb);
    if (enca_val == encb_val) tickCount = (tickCount-1+TICKS_PER_REVOLUTION)%TICKS_PER_REVOLUTION;
    else tickCount = (tickCount+1)%TICKS_PER_REVOLUTION;
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
      direction = DIR_CW;
      break;
    }
  }

  int getState() {
    return state;
  }

  /*!
    if pos is unspecified (-1), just move in the given direction
   */
  void setTargetPosition(int pos, Direction dir) {
    targetPosition = pos;
    direction = dir;
  }

  int getPosition() {
    return actualPosition;
  }

  void readSensors() {
    magnetValue = analogRead(homeSensor);
    // if (magnetValue < 300 && magnetValue > 20) {
    //   Serial.print(id);Serial.print(" ");
    //   Serial.print(magnetValue);Serial.println();
    // }
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
    return magnetValue < MAGNET_SENSITIVITY;
  }

  bool isHome() {
    return (magnetActive(magnetValue) && !magnetActive(lastMagnetValue));
  }

  void handleCount() {
    uint16_t pos = ticksToPos(tickCount);
    if (pos != actualPosition) {
      actualPosition = pos;
      if (global_state == STATE_READY) {
        Serial.print("DISK ");Serial.print(id);
        Serial.print(" POS ");Serial.print(actualPosition);
        Serial.print(" DIR ");Serial.print(direction);
        Serial.print(" TICKS ");Serial.print(tickCount);
        Serial.println();
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
        tickCount = 0;
      }
    }
    else if (state == DISK_READY) {
      handleCount();

      if (targetPosition >= 0 && actualPosition == targetPosition) {
        direction = DIR_OFF;
        targetPosition = -1;
      }

      if (magnetActive(magnetValue) && !magnetActive(lastMagnetValue)) {
        // Hit left side of magnet
        Serial.print(id);
        Serial.print(" HIT TICKS " );
        Serial.println(tickCount);
        lastHitTicks = tickCount;
      }
      else if (!magnetActive(magnetValue) && magnetActive(lastMagnetValue)) {
        // Exited left side of magnet
        Serial.print(id);
        Serial.print(" EXIT TICKS " );
        Serial.println(tickCount);
        lastExitTicks = tickCount;
      }

    }
    motorMove();

    lastMagnetValue = magnetValue;
  }
};

Disk disk[3] = {
  Disk(0, DISK0_HOME_SENSOR, DISK0_ENC_A, DISK0_ENC_B, DISK0_MOTOR_A, DISK0_MOTOR_B, &disk0a_isr, &disk0b_isr),
  Disk(1, DISK1_HOME_SENSOR, DISK1_ENC_A, DISK1_ENC_B, DISK1_MOTOR_A, DISK1_MOTOR_B, &disk1a_isr, &disk1b_isr),
  Disk(2, DISK2_HOME_SENSOR, DISK2_ENC_A, DISK2_ENC_B, DISK2_MOTOR_A, DISK2_MOTOR_B, &disk2a_isr, &disk2b_isr)
};

void disk0a_isr() {
  cli();
  disk[0].handleEncA();
  sei();
}
void disk0b_isr() {
  cli();
  disk[0].handleEncB();
  sei();
}
void disk1a_isr() {
  cli();
  disk[1].handleEncA();
  sei();
}
void disk1b_isr() {
  cli();
  disk[1].handleEncB();
  sei();
}
void disk2a_isr() {
  cli();
  disk[2].handleEncA();
  sei();
}
void disk2b_isr() {
  cli();
  disk[2].handleEncB();
  sei();
}

void setGlobalState(int state) {
  global_state = state;
  if (global_state == STATE_HOMING) {
    for (int i=0;i<3;i++) disk[i].setTargetPosition(-1, DIR_CCW);
    homingIdx = timer.after(2000, startHoming, NULL);
  }
}

void startHoming(void *context) {
  blinkerIdx = timer.every(500, globalBlink, NULL);
  for (int i=0;i<3;i++) disk[i].setState(Disk::DISK_HOMING);
  homingIdx = -1;
}

void globalBlink(void *context) {
  if (global_state == STATE_HOMING) {
    uint32_t t = millis() / 100;
    const CRGB &c = (t % 2) ? BLACK : RED;
    for (int i=0;i<3;i++) {
      if (disk[i].getState() == Disk::DISK_HOMING) strip.setColor(i, c);
      else strip.setColor(i, GREEN);
    }
    FastLED.show();
  }
}

void manageDiskGame() {
  if (global_state == STATE_HOMING) {
    bool homed = true;
    for (int i=0;i<3;i++) {
      if (disk[i].getState() == Disk::DISK_HOMING) homed = false;
    }
    if (homed) {
      for (int i=0;i<3;i++) {
        Serial.print(disk[i].id);
        Serial.print(" HOME TICKS ");
        Serial.println(disk[i].tickCount);
      }
      timer.stop(blinkerIdx);
      blinkerIdx = -1;
      LEDStripInterface::setAllColors(GREEN);
      delay(1000);
      setGlobalState(STATE_READY);
      LEDStripInterface::setAllColors(BLACK);
    }
  }

  for (int i=0;i<3;i++) disk[i].manage();
}

void setup()
{
  Serial.begin(115200);

  resetInterface(DEBUG_MODE);
  setupCommands();

  //  attachInterrupt(DISK0_ENC_A, disk0a_isr, CHANGE);

}

// Reset everything to initial state
void resetInterface(bool debug)
{
  for (int i=0;i<3;i++) disk[i].setup();
  for (int i=0;i<LEDStripInterface::getNumStrips();i++) LEDStripInterface::getStrip(i).setup();
  LEDStripInterface::setAllColors(BLACK);
  setGlobalState(STATE_READY);

  global_initialized = false;
  global_debug = debug;
  global_userid = -1;

#ifdef AUTOINIT
  global_initialized = true;
  global_userid = 0;
#endif

  Serial.println();
  Serial.println("HELLO disk");
  if (global_debug) Serial.println("DEBUG disk");
}

void initInterface() {
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
  disk[diskid].setTargetPosition(pos, dir);
}

void do_panel_set(uint8_t strip, uint8_t panel, uint8_t intensity, const CRGB &color, AnywareEasing::EasingType  easing)
{
  CRGB newcol = applyIntensity(color, intensity);
  Serial.print(strip);Serial.print(" ");Serial.println(panel);
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
    s.ease(p.pixelid, easing, newcol);
  }
}

void do_panel_pulse(uint8_t strip, uint8_t panel, uint8_t intensity, const CRGB &color, AnywareEasing::EasingType  easing)
{
  do_panel_set(strip, panel, intensity, color, easing);
}

void do_panel_intensity(uint8_t strip, uint8_t intensity)
{
  // FIXME: We don't currently support per-strip intensity, only global intensity
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

  if (!global_initialized) return;

  timer.update();
  handleAnimations();
  manageDiskGame();
}

