// Yes, this is actually -*-c++-*-

/*!
  This sketch is responsible for the Disk game

  Example commands:

DISK-RESET
DISK-STATE success
DISK 0 POS 10 DIR 1 USER 1
DISK 1 POS 50 DIR 1 USER 1
DISK 2 POS 60 DIR 1 USER 2
DISK 0 DIR 0 USER 1
DISK 0 DIR 1 USER 1
DISK 0 DIR -1 USER 1
DISK 0 DIR 1 USER 1
DISK 1 DIR -1 USER 1
DISK 2 DIR 1 USER 1
DISK-EXIT

*/

#if !defined(__MK20DX256__)
#error Oops!  Make sure you have 'Teensy 3.1' selected from the 'Tools -> Board' menu.
#endif

#include "./Timer.h"
#include "./Bounce2.h"
#include "DiskInterface.h"
#include "anyware_serial.h"

// Set to 1 to start in debug mode
#define DEBUG_MODE 0

// Bottom disk
const int DISK0_HOME_SENSOR = A0;
const int DISK0_ENC_A = 5;
const int DISK0_ENC_B = 6;
const int DISK0_MOTOR_A = 4;
const int DISK0_MOTOR_B = 3;
const int DISK0_TICKS_PER_REVOLUTION = 30089;

// Middle disk
const int DISK1_HOME_SENSOR = A1;
const int DISK1_ENC_A = 9;
const int DISK1_ENC_B = 10;
const int DISK1_MOTOR_A = 8;
const int DISK1_MOTOR_B = 7;
const int DISK1_TICKS_PER_REVOLUTION = 31348;

// Top disk
const int DISK2_HOME_SENSOR = A2;
const int DISK2_ENC_A = 20;
const int DISK2_ENC_B = 19;
const int DISK2_MOTOR_A = 17;
const int DISK2_MOTOR_B = 18;
const int DISK2_TICKS_PER_REVOLUTION = 31385;

// Globals

const uint8_t MAGNET_SENSITIVITY = 160;

#define USER0_STR "0"
#define USER1_STR "1"
#define USER2_STR "2"

Timer timer;
int8_t homingIdx = -1;

struct Disk {
  int id;
  int homeSensor;
  int enca, encb;
  int motorA,motorB;
  uint32_t ticksPerRevolution;

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

  Disk(int diskid, int hs, int enca, int encb, int ma, int mb, uint32_t ticks, void (*enca_isr)(), void (*encb_isr)()) :
    id(diskid), homeSensor(hs), enca(enca), encb(encb), motorA(ma), motorB(mb),
    ticksPerRevolution(ticks),
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
    if (ticks < 0) ticks += ticksPerRevolution;
    return (ticks * 360 / ticksPerRevolution) % 360;
  }
  
  inline void handleEncA() {
    enca_val = digitalRead(enca);
    if (enca_val != encb_val) tickCount = (tickCount-1+ticksPerRevolution)%ticksPerRevolution;
    else tickCount = (tickCount+1)%ticksPerRevolution;
  }

  inline void handleEncB() {
    encb_val = digitalRead(encb);
    if (enca_val == encb_val) tickCount = (tickCount-1+ticksPerRevolution)%ticksPerRevolution;
    else tickCount = (tickCount+1)%ticksPerRevolution;
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
        tickCount = 0;
        actualPosition = 0;
        setState(DISK_READY);
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
        int32_t tmpticks = tickCount;
        if (global_state == STATE_READY && this->direction != DIR_OFF) {
          Serial.print("DEBUG DISK ");
          Serial.print(id);
          Serial.print(" HIT TICKS " );
          Serial.println(tmpticks);
        }
        lastHitTicks = tmpticks;
      }
      else if (!magnetActive(magnetValue) && magnetActive(lastMagnetValue)) {
        // Exited left side of magnet
        // Serial.print(id);
        // Serial.print(" EXIT TICKS " );
        // Serial.println(tickCount);
        lastExitTicks = tickCount;
      }

    }
    motorMove();

    lastMagnetValue = magnetValue;
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

Disk disk[3] = {
  Disk(0, DISK0_HOME_SENSOR, DISK0_ENC_A, DISK0_ENC_B, DISK0_MOTOR_A, DISK0_MOTOR_B, DISK0_TICKS_PER_REVOLUTION, &disk0a_isr, &disk0b_isr),
  Disk(1, DISK1_HOME_SENSOR, DISK1_ENC_A, DISK1_ENC_B, DISK1_MOTOR_A, DISK1_MOTOR_B, DISK1_TICKS_PER_REVOLUTION, &disk1a_isr, &disk1b_isr),
  Disk(2, DISK2_HOME_SENSOR, DISK2_ENC_A, DISK2_ENC_B, DISK2_MOTOR_A, DISK2_MOTOR_B, DISK2_TICKS_PER_REVOLUTION, &disk2a_isr, &disk2b_isr)
};

void setGlobalState(int state) {
  if (state == STATE_HOMING) {
    global_state = STATE_BLOCKED; // Don't output anything before we start homing
    // Move out of home zone for a bit to make sure we re-trigger the sensors
    for (int i=0;i<3;i++) disk[i].setTargetPosition(-1, DIR_CCW);
    homingIdx = timer.after(2000, startHoming, NULL);
  }
  else {
    global_state = state;
  }
}

void startHoming(void *context) {
  global_state = STATE_HOMING;
  for (int i=0;i<3;i++) disk[i].setState(Disk::DISK_HOMING);
  homingIdx = -1;
}

void manageDiskGame() {
  if (global_state == STATE_HOMING) {
    bool homed = true;
    for (int i=0;i<3;i++) {
      if (disk[i].getState() == Disk::DISK_HOMING) homed = false;
    }
    if (homed) {
      // for (int i=0;i<3;i++) {
      //   Serial.print(disk[i].id);
      //   Serial.print(" HOME TICKS ");
      //   Serial.println(disk[i].tickCount);
      // }
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
  disk[diskid].setTargetPosition(pos, dir);
}

void loop()
{
  handleSerial();

  timer.update();
  manageDiskGame();
}

