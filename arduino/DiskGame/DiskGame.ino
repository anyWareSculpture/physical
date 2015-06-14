// Yes, this is actually -*-c++-*-

/*!
  This sketch is responsible for the Disk game

  State flags:
  * disk      Disk game active
*/

#include "./Timer.h"
#include "./Bounce2.h"
#include "DiskGame.h"

/*
  Serial protocol:

  INIT <name>
  STATE <state>
  DISK <disk> STATE <state>
  DISK <disk> POS <pos>

  Example commands:

IDENTITY 0
DISK-INIT
DISK-RESET
DISK-STATE success
DISK 0 POS 10 DIR 1 USER 1
DISK 1 POS 50 DIR 1 USER 1
DISK 2 POS 60 DIR 1 USER 2
DISK 0 DIR 0 USER 1
DISK-EXIT

 */

// Colors
uint32_t RED = Color(255,0,0);
uint32_t GREEN = Color(0,255,0);
uint32_t BLACK = Color(0,0,0);
uint32_t MYBLUE = Color(0,20,147);
uint32_t MYYELLOW = Color(255, 255, 0);
uint32_t MYPINK = Color(255,20,147);

const uint32_t locationColor[3] = {
  MYBLUE,
  MYYELLOW,
  MYPINK
};

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

const int DISK_GAME_STATE_OFF = 0;
const int DISK_GAME_STATE_READY = 1;
const int DISK_GAME_STATE_HOMING = 2;
const int DISK_GAME_STATE_SUCCESS = 3;

int diskGameState = DISK_GAME_STATE_OFF;
Timer timer;
int8_t blinkerIdx = -1;

int combination[3] = {17,43,63};

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

  uint8_t localDirection;

  int remoteUser;
  int remotePosition;
  uint8_t remoteDirection;
  bool followRemote;

  uint32_t currColor;
  int actualPosition;
  uint8_t actualDirection;
  bool moving;

  static const int STATE_OFF = 0;
  static const int STATE_READY = 1;
  static const int STATE_HOMING = 2;
  int state;

  Disk(int diskid,
       int ls, int rs, int hs, int cs,
       int r, int g, int b,
       int ma, int mb) :
    id(diskid), 
    leftSensor(ls), rightSensor(rs), homeSensor(hs), countSensor(cs),
    redPin(r), greenPin(g), bluePin(b), 
    motorA(ma), motorB(mb),

    magnetValue(0), lastMagnetValue(0),
    localDirection(DIR_OFF),
    actualPosition(0), actualDirection(DIR_OFF), moving(false), 
    state(STATE_OFF)
  {
    leftIR.attach(leftSensor);
    leftIR.interval(10);
    rightIR.attach(rightSensor);
    rightIR.interval(10);
    countState.attach(countSensor);
    countState.interval(50);
  }

  void setup() {
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);
    pinMode(motorA, OUTPUT);
    pinMode(motorB, OUTPUT);
    pinMode(leftSensor,INPUT_PULLUP);
    pinMode(rightSensor,INPUT_PULLUP);
    pinMode(countSensor,INPUT_PULLUP);
    reset();
  }

  void reset() {
    setColor(BLACK);

    remoteUser = -1;
    remotePosition = 0;
    remoteDirection = DIR_OFF;
    followRemote = false;
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

  void setState(int s) {
    state = s;
    if (state == STATE_OFF) {
      moving = false;
    }
    // We don't have a per-disk state at the moment
    /*
    Serial.print("DISK ");Serial.print(id);Serial.print(" STATE ");
    switch (state) {
    case STATE_OFF:
      Serial.println("Off");
      break;
    case STATE_READY:
      Serial.println("Interactive");
      break;
    case STATE_HOMING:
      Serial.println("Homing");
      break;
    }
    */
  }

  int getState() {
    return state;
  }

  /*!
    if pos is unspecified (-1), don't follow remote, just use remote direction as a blocker if necessary
   */
  void setRemotePosition(int userid, int pos, int dir) {
    remoteUser = userid;
    remoteDirection = dir;
    if (pos >= 0) {
      remotePosition = pos;
      followRemote = true;
    }
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
    if (moving) {
      if (actualDirection == DIR_CW) {
        digitalWrite(motorA, HIGH); 
        digitalWrite(motorB, LOW); //1A high and 2A low = turn right
      }
      else if (actualDirection == DIR_CCW) {
        digitalWrite(motorA, LOW); 
        digitalWrite(motorB, HIGH); //1A low and 2A high = turn left  
      }
    }
    else {
      digitalWrite(motorA, LOW); 
      digitalWrite(motorB, LOW); //1A low and 2A low = off  
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
      if (actualDirection == DIR_CW) actualPosition = (actualPosition + REV - 1) % REV;
      else if (actualDirection == DIR_CCW) actualPosition = (actualPosition + 1) % REV;
      Serial.print("DISK ");Serial.print(id);
      Serial.print(" POS ");Serial.print(actualPosition);
      Serial.print(" DIR ");Serial.println(actualDirection);
    }
  }

  void manage() {

    // Read local sensors
    readSensors();

    if (state == STATE_HOMING) {
      if (isHome()) {
        setState(STATE_OFF);
        actualPosition = 0;
      }
      else {
        actualDirection = DIR_CCW;
        moving = true;
      }
    }
    else if (state == STATE_READY) {
      handleCount();

      localDirection = DIR_OFF;
      if (rightIR.read()) localDirection |= DIR_CW;
      if (leftIR.read()) localDirection |= DIR_CCW;

      // If position is correct, attempt to move local disk in the actual direction
      // FIXME: Do we need to measure "close enough" to avoid oscillations?
      if (followRemote && remotePosition == actualPosition) followRemote = false;

      if (followRemote) {
        actualDirection = remoteDirection;
      }
      else {
        actualDirection = localDirection;
        // ..but remote can block
        if ((actualDirection | remoteDirection) == DIR_BLOCKED) actualDirection = DIR_BLOCKED;
      }

      switch (actualDirection) {
      case DIR_OFF:
        moving = false;
        setColor(BLACK);
        break;
      case DIR_CCW:
        moving = true;
        setColor(followRemote ? locationColor[remoteUser] : locationColor[global_userid]);
        break;
      case DIR_CW:
        moving = true;
        setColor(followRemote ? locationColor[remoteUser] : locationColor[global_userid]);
        break;
      case DIR_BLOCKED:
        moving = false;
        setColor(RED);
        break;
      }
    }
    motorMove();

    lastMagnetValue = magnetValue;
  }
};

Disk disk[3] = {
  Disk(0, DISK0_LEFT_SENSOR, DISK0_RIGHT_SENSOR, DISK0_HOME_SENSOR, DISK0_COUNT_SENSOR,
       DISK0_RED_PIN, DISK0_GREEN_PIN, DISK0_BLUE_PIN,
       DISK0_MOTOR_A, DISK0_MOTOR_B),
  Disk(1, DISK1_LEFT_SENSOR, DISK1_RIGHT_SENSOR, DISK1_HOME_SENSOR, DISK1_COUNT_SENSOR,
       DISK1_RED_PIN, DISK1_GREEN_PIN, DISK1_BLUE_PIN,
       DISK1_MOTOR_A, DISK1_MOTOR_B),
  Disk(2, DISK2_LEFT_SENSOR, DISK2_RIGHT_SENSOR, DISK2_HOME_SENSOR, DISK2_COUNT_SENSOR,
       DISK2_RED_PIN, DISK2_GREEN_PIN, DISK2_BLUE_PIN,
       DISK2_MOTOR_A, DISK2_MOTOR_B)
};

void SetUpIR () {
  // Pin9 is OC2B
  pinMode(9, OUTPUT);  //IR LED output
  TCCR2A = _BV(COM2B0) | _BV(WGM21);
  TCCR2B = _BV(CS20);
  OCR2A = 209;
}

void setDiskGameState(int state) {
  diskGameState = state;
  if (state == DISK_GAME_STATE_HOMING) {
    Serial.println("DISK-STATE homing");
    blinkerIdx = timer.every(500, globalBlink, NULL);
    for (int i=0;i<3;i++) disk[i].setState(Disk::STATE_HOMING);
  }
  else if (state == DISK_GAME_STATE_READY) {
    Serial.println("DISK-STATE ready");
    for (int i=0;i<3;i++) disk[i].setState(Disk::STATE_READY);
  }
  else if (state == DISK_GAME_STATE_SUCCESS) {
    Serial.println("DISK-STATE success");
    for (int i=0;i<3;i++) disk[i].setState(Disk::STATE_OFF);
    blinkerIdx = timer.every(500, successBlink, NULL);
  }
}

void globalBlink(void *context) {
  if (diskGameState == DISK_GAME_STATE_HOMING) {
    uint32_t t = millis() / 100;
    uint32_t c = (t % 2) ? BLACK : RED;
    for (int i=0;i<3;i++) {
      if (disk[i].getState() == Disk::STATE_HOMING) disk[i].setColor(c);
      else disk[i].setColor(GREEN);
    }
  }
}

void successBlink(void *context) {
  uint32_t t = millis() / 100;
  uint32_t c = (t % 2) ? BLACK : GREEN;
  for (int i=0;i<3;i++) disk[i].setColor(c);
}

void manageDiskGame() {
  if (diskGameState == DISK_GAME_STATE_HOMING) {
    bool homed = true;
    for (int i=0;i<3;i++) {
      if (disk[i].getState() == Disk::STATE_HOMING) homed = false;
    }
    if (homed) {
      timer.stop(blinkerIdx);
      blinkerIdx = -1;
      for (int i=0;i<3;i++) disk[i].setColor(GREEN);
      delay(1000);
      setDiskGameState(DISK_GAME_STATE_READY);
    }
  }
  else if (diskGameState == DISK_GAME_STATE_READY) {
    handleCombination(false);
  }

  for (int i=0;i<3;i++) disk[i].manage();
}

void handleCombination(bool absolute) {
  bool success = false;
  if (absolute) {
    success = abs(disk[0].getPosition() - combination[0]) <= 1 &&
      abs(disk[1].getPosition() - combination[1]) <= 1 &&
      abs(disk[2].getPosition() - combination[2]) <= 1;
  }
  else {
    success =
      abs(((disk[1].getPosition() - disk[0].getPosition() + REV) % REV) -
          ((combination[1] - combination[0] + REV) % REV)) <= 1 &&
      abs(((disk[2].getPosition() - disk[1].getPosition() + REV) % REV) -
          ((combination[2] - combination[1] + REV) % REV)) <= 1;
  }
  if (success) {
    setDiskGameState(DISK_GAME_STATE_SUCCESS);
  }
}

void setup()
{
  Serial.begin(115200);

  setupCommands();
  reset(DEBUG_MODE);
}

// Reset everything to initial state
void reset(bool debug)
{
  SetUpIR();
  for (int i=0;i<3;i++) {
    disk[i].setup();
  }
  setDiskGameState(DISK_GAME_STATE_OFF);

  global_debug = debug;
  global_userid = -1;
  global_state = STATE_IDLE;

  Serial.println();
  Serial.println("HELLO disk");
  if (global_debug) Serial.println("DEBUG disk");
}

void loop()
{
  timer.update();
  handleSerial();

  if (global_state & STATE_DISK) {
    manageDiskGame();
  }
}

/*!
  DISK-INIT

  Initialize disk game. This just allows the Arduino to listen to DISK commands.
 */
void do_disk_init()
{
  global_state |= STATE_DISK;
}

/*!
  DISK-EXIT

  Exit disk game. Stop any currently running animation or process.
  No longer listen to DISK commands.
*/
void do_disk_exit()
{
  global_state &= ~STATE_DISK;

  timer.stop(blinkerIdx);
  for (int i=0;i<3;i++) disk[i].reset();
  setDiskGameState(DISK_GAME_STATE_OFF);
}

/*!
  DISK-RESET

  Resets the game, home the disks and afterwards to into "ready" mode.
*/
void do_disk_reset()
{
  timer.stop(blinkerIdx);
  for (int i=0;i<3;i++) disk[i].reset();
  setDiskGameState(DISK_GAME_STATE_HOMING);
}

/*!
  DISK-STATE <state>  (state = "success")

  Only supports the "success" state. Show success light pattern/animation.
 */
void do_disk_state()
{
  setDiskGameState(DISK_GAME_STATE_SUCCESS);
}

/*!
  pos can be unspecified (-1)
*/
void do_disk(uint8_t diskid, int userid, int pos, DirectionFlags dir)
{
  disk[diskid].setRemotePosition(userid, pos, dir);
}
