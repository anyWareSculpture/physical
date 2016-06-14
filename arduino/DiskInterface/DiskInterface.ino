// Yes, this is actually -*-c++-*-

/*!
  This sketch is responsible for the Disk game

  Example commands:

HELLO
DISK-RESET
DISK 0 POS 10 DIR 1
DISK 1 POS 50 DIR 1
DISK 2 POS 60 DIR 1
DISK 0 DIR 0 USER 1
DISK 0 DIR 1 USER 1
DISK 0 DIR -1 USER 1
DISK 0 DIR 1 USER 1
DISK 1 DIR -1 USER 1
DISK 2 DIR 1 USER 1

*/

#if !defined(__MK20DX256__)
#error Oops!  Make sure you have 'Teensy 3.1' selected from the 'Tools -> Board' menu.
#endif

#include "DiskInterface.h"
#include "anyware_serial.h"

// Set to 1 to start in debug mode
#define DEBUG_MODE 0

// All disks
const int STEPS_PER_REVOLUTION = 180;

// Bottom disk
const int DISK0_STEP_SENSOR = 2;
const int DISK0_ENC_A = 17;
const int DISK0_ENC_B = 16;
const int DISK0_MOTOR_A = 21;
const int DISK0_MOTOR_B = 20;
const int DISK0_TICKS_PER_STEP = 36;

// Middle disk
const int DISK1_STEP_SENSOR = 1;
const int DISK1_ENC_A = 11;
const int DISK1_ENC_B = 12;
const int DISK1_MOTOR_A = 3;
const int DISK1_MOTOR_B = 5;
const int DISK1_TICKS_PER_STEP = 56;

// Top disk
const int DISK2_STEP_SENSOR = 0;
const int DISK2_ENC_A = 19;
const int DISK2_ENC_B = 18;
const int DISK2_MOTOR_A = 22;
const int DISK2_MOTOR_B = 23;
const int DISK2_TICKS_PER_STEP = 36;

// Globals

const uint8_t MAGNET_SENSITIVITY = 160;

#define USER0_STR "0"
#define USER1_STR "1"
#define USER2_STR "2"

struct Disk {
  int id;
  int stepSensor;
  int enca, encb;
  int motorA,motorB;
  uint16_t ticksPerStep;

  void (*enca_isr)();
  void (*encb_isr)();
  void (*step_isr)();

  bool enca_val;
  bool encb_val;
  bool step_val;

  int16_t targetPosition;
  uint16_t actualPosition;
  int16_t tickCount;
  int16_t lastTickCount;
  int16_t tickDir;
  int16_t stepCount;

  Direction direction;

  static const int DISK_READY = 0;
  static const int DISK_HOMING = 1;
  int state;
  uint8_t home;

  Disk(int diskid, int stepsensor, int enca, int encb, int ma, int mb, uint16_t ticks,
       void (*enca_isr)(), void (*encb_isr)(), void (*step_isr)()) :
    id(diskid), stepSensor(stepsensor), enca(enca), encb(encb), motorA(ma), motorB(mb),
    ticksPerStep(ticks),
    enca_isr(enca_isr), encb_isr(encb_isr), step_isr(step_isr),
    enca_val(false), encb_val(false),
    actualPosition(0), tickCount(0), tickDir(DIR_OFF), lastTickCount(0), stepCount(0),
    direction(DIR_OFF), state(DISK_READY), home(0)
  {
  }

  void setup() {
    pinMode(motorA, OUTPUT);
    pinMode(motorB, OUTPUT);
    pinMode(enca, INPUT_PULLUP);
    pinMode(encb, INPUT_PULLUP);
    pinMode(stepSensor, INPUT_PULLUP);
    enca_val = digitalRead(enca);
    encb_val = digitalRead(encb);
    reset();
    attachInterrupt(enca, enca_isr, CHANGE);
    attachInterrupt(encb, encb_isr, CHANGE);
    attachInterrupt(stepSensor, step_isr, CHANGE);
  }

  void reset() {
    targetPosition = -1;
    Serial.println("Reset");
    direction = DIR_OFF;
  }

  inline uint16_t calcPos() {
    return (stepCount * 360 / STEPS_PER_REVOLUTION + tickCount * (360 / STEPS_PER_REVOLUTION) / ticksPerStep + 360)%360;
  }
  
  inline void handleEncA() {
    enca_val = digitalRead(enca);
    if (enca_val != encb_val) {
      tickCount++;
      tickDir = DIR_CCW;
    }
    else {
      tickCount--;
      tickDir = DIR_CW;
    }
  }

  inline void handleEncB() {
    encb_val = digitalRead(encb);
    if (enca_val == encb_val) {
      tickCount++;
      tickDir = DIR_CCW;
    }
    else {
      tickCount--;
      tickDir = DIR_CW;
    }
  }

  inline void handleStep() {
    if (abs(tickCount) > ticksPerStep * 2) {
      if (tickCount > 0) stepCount = 0;
      else stepCount = STEPS_PER_REVOLUTION-3;
      tickCount = 0;
      if (state == DISK_HOMING) home++;
    }
    else {
      if (tickDir == DIR_CCW) stepCount++;
      else if (tickDir == DIR_CW) stepCount--;
      lastTickCount = tickCount;
      tickCount = 0;
    }
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
      setTargetPosition(-1, DIR_CW);
      home = 0;
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

  void handleCount() {
    uint16_t pos = calcPos();
    if (pos != actualPosition) {
      actualPosition = pos;
      if (global_state == STATE_READY) {
        Serial.print("DISK ");Serial.print(id);
        Serial.print(" POS ");Serial.print(actualPosition);
        Serial.print(" DIR ");Serial.print(direction);
        Serial.print(" STEPS ");Serial.print(stepCount);
        Serial.print(" TICKS ");Serial.print(tickCount);
        Serial.println();
      }
    }
  }

  void manage() {
    if (state == DISK_HOMING) {
      if (home == 1) {
        direction = DIR_CCW;
      }
      else if (home == 2) {
        actualPosition = 0;
        setState(DISK_READY);
      }
    }
    else if (state == DISK_READY) {
      handleCount();

      if (targetPosition >= 0 && actualPosition == targetPosition) {
        Serial.println("at wanted position");
        direction = DIR_OFF;
        targetPosition = -1;
      }
    }
    motorMove();
  }
};

extern Disk disk[3];

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
void disk0step_isr() {
  cli();
  disk[0].handleStep();
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
void disk1step_isr() {
  cli();
  disk[1].handleStep();
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
void disk2step_isr() {
  cli();
  disk[2].handleStep();
  sei();
}

Disk disk[3] = {
  Disk(0, DISK0_STEP_SENSOR, DISK0_ENC_A, DISK0_ENC_B, DISK0_MOTOR_A, DISK0_MOTOR_B, DISK0_TICKS_PER_STEP, &disk0a_isr, &disk0b_isr, &disk0step_isr),
  Disk(1, DISK1_STEP_SENSOR, DISK1_ENC_A, DISK1_ENC_B, DISK1_MOTOR_A, DISK1_MOTOR_B, DISK1_TICKS_PER_STEP, &disk1a_isr, &disk1b_isr, &disk1step_isr),
  Disk(2, DISK2_STEP_SENSOR, DISK2_ENC_A, DISK2_ENC_B, DISK2_MOTOR_A, DISK2_MOTOR_B, DISK2_TICKS_PER_STEP, &disk2a_isr, &disk2b_isr, &disk2step_isr)
};

void setGlobalState(int state) {
  if (state == STATE_HOMING) {
    global_state = STATE_BLOCKED; // Don't output anything before we start homing
    startHoming();
  }
  else {
    global_state = state;
  }
}

void startHoming() {
  global_state = STATE_HOMING;
  for (int i=0;i<3;i++) disk[i].setState(Disk::DISK_HOMING);
}

void manageDiskGame() {
  if (global_state == STATE_HOMING) {
    bool homed = true;
    for (int i=0;i<3;i++) {
      if (disk[i].getState() == Disk::DISK_HOMING) homed = false;
    }
    if (homed) {
      setGlobalState(STATE_READY);
    }
  }

  for (int i=0;i<3;i++) disk[i].manage();
}

void setup()
{
  Serial.begin(115200);

  resetInterface(DEBUG_MODE);
  setupCommands();
}

// Reset everything to initial state
void resetInterface(bool debug)
{
  for (int i=0;i<3;i++) disk[i].setup();
  setGlobalState(STATE_READY);

  global_debug = debug;

  Serial.println();
  Serial.println("HELLO disk");
  if (global_debug) Serial.println("DEBUG disk");
  printCommands();
}

void initInterface() {
}

/*!
  DISK-RESET

  Home the disks and afterwards go into "ready" mode.
*/
void do_disk_reset()
{
  for (int i=0;i<3;i++) disk[i].reset();
  setGlobalState(STATE_HOMING);
}

/*!
  pos can be unspecified (-1)
*/
void do_disk(uint8_t diskid, int userid, int pos, Direction dir)
{
  // Ignore pos/dir commands while homing
  if (disk[diskid].getState() == Disk::DISK_READY) {
    disk[diskid].setTargetPosition(pos, dir);
  }
}

void loop()
{
  handleSerial();

  manageDiskGame();
}

