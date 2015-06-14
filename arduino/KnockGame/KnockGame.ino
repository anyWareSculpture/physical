// Yes, this is actually -*-c++-*-

/*!
  This sketch is responsible for the Handshake and Knock interactions

  State flags:
  * handshake      Handshake active
  * knock          Knocking game active


  Example commands:
IDENTITY 0
KNOCK-INIT
KNOCK-PATTERN 0 447 318 152 450
KNOCK-ECHO 1 1000 300 1000 200
KNOCK-ECHO 1 0 300 300
KNOCK-EXIT


*/

#include "knocker.h"
#include "SerialCommand.h"
#include "KnockGame.h"

#define SOLENOID_PIN 4
#define PIEZO_PIN 1

// Used for handshake for now
#define BUTTON_PIN 3

// Lit while knocking
#define LED1_PIN 5
// Lit while knocking game is active
#define LED2_PIN 6
// Unused
#define LED3_PIN 7

// Currently used for vibration on EXIT knock
#define BUZZER_PIN 8

#define HANDSHAKE_STR "handshake"
#define KNOCK_STR "knock"

#define USER0_STR "0"
#define USER1_STR "1"
#define USER2_STR "2"

int global_state = STATE_IDLE;
bool global_debug = false;

const int KNOCK_STRENGTH = 40;
const int ECHO_STRENGTH = 17;

unsigned int knock_pattern[MAX_PATTERN_LENGTH];
uint8_t knock_pattern_count = 0;

unsigned int recorded_pattern[MAX_PATTERN_LENGTH];

Knocker knocker(PIEZO_PIN, SOLENOID_PIN);

// Send pattern over serial
void send_knock_response(unsigned int *pattern, uint8_t length)
{
  Serial.print("KNOCK-RESPONSE");
  for (uint8_t i=0;i<length;i++) {
    Serial.print(" ");
    Serial.print(pattern[i]);
  }
  Serial.println();
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
  pinMode(SOLENOID_PIN, OUTPUT);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
 
  digitalWrite(LED1_PIN, 0);
  digitalWrite(LED2_PIN, 0);
  digitalWrite(LED3_PIN, 0);
  digitalWrite(BUZZER_PIN, 0);

  global_debug = debug;
  global_userid = -1;
  global_state = STATE_HANDSHAKE; // Handshake is always on
  Serial.println();
  Serial.print("HELLO ");
  Serial.print(HANDSHAKE_STR);
  Serial.print(" ");
  Serial.println(KNOCK_STR);
  if (global_debug) {
    Serial.print("DEBUG ");
    Serial.print(HANDSHAKE_STR);
    Serial.print(" ");
    Serial.println(KNOCK_STR);
  }
}

// Detect and send knock patterns
void handleKnock()
{
  int len = knocker.detectPattern(recorded_pattern, MAX_PATTERN_LENGTH, 2000);
  if (len > 0) { // Detected full pattern
    send_knock_response(recorded_pattern, len);
  }
}

// Detect and send handshake
bool buttonState = false;
void handleHandshake()
{
  bool currButtonState = digitalRead(BUTTON_PIN);
  if (currButtonState != buttonState) {
    buttonState = currButtonState;
    Serial.print("HANDSHAKE ");
    Serial.println(buttonState ? "1" : "0");
  }
}

void do_knock_init()
{
  global_state |= STATE_KNOCK;
  digitalWrite(LED2_PIN, 1);
}

void do_knock_exit()
{
  global_state &= ~STATE_KNOCK;
  digitalWrite(LED2_PIN, 0);
  // FIXME: We simulate the transition for now
  digitalWrite(BUZZER_PIN, 1);
  delay(2000);
  digitalWrite(BUZZER_PIN, 0);
}

void do_knock_pattern()
{
  digitalWrite(LED1_PIN, 1);
  digitalWrite(LED2_PIN, 0);
  knocker.playPattern(knock_pattern, knock_pattern_count, KNOCK_STRENGTH);
  digitalWrite(LED1_PIN, 0);
  digitalWrite(LED2_PIN, 1);
}

void do_knock_echo()
{
  digitalWrite(LED1_PIN, 1);
  digitalWrite(LED2_PIN, 0);
  knocker.playPattern(knock_pattern, knock_pattern_count, ECHO_STRENGTH);
  digitalWrite(LED1_PIN, 0);
  digitalWrite(LED2_PIN, 1);
}

void loop()
{
  handleSerial();

  if (global_state & STATE_KNOCK) handleKnock();
  if (global_state & STATE_HANDSHAKE) handleHandshake();

  // FIXME: Handle fading of handshake indicators
}
