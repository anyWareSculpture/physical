// Yes, this is actually -*-c++-*-

/*!
  This sketch is responsible for the Handshake and Knock interactions

  State flags:
  * Handshake      Handshake active
  * Knock          Knocking game active
*/


#include "knocker.h"
#include "SerialCommand.h"

// Set to 1 to start in debug mode
#define DEBUG_MODE 1

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

// State flags
#define STATE_IDLE      0x00
#define STATE_HANDSHAKE 0x01
#define STATE_KNOCK     0x02

int global_state = STATE_IDLE;
int global_userid = -1;
bool global_debug = false;

const int MAX_PATTERN_LENGTH = 50;
unsigned int knock_pattern[MAX_PATTERN_LENGTH];
unsigned int knock_pattern_count = 0;

unsigned int recorded_pattern[MAX_PATTERN_LENGTH];

Knocker knocker(PIEZO_PIN, SOLENOID_PIN);
SerialCommand sCmd;

void error(const char *type, const char *msg)
{
  Serial.print("ERROR ");
  Serial.print(type);
  Serial.print(": ");
  Serial.println(msg);
}

int getUserIdArg(const char *arg)
{
  if (!strcmp(arg, USER0_STR)) return 0;
  else if (!strcmp(arg, USER1_STR)) return 1;
  else if (!strcmp(arg, USER2_STR)) return 2;
  else return -1;
}

void unrecognized(const char *cmd)
{
  error("unrecognized command", cmd);
}

void reset_action()
{
  char *debugarg = sCmd.next();
  bool debug = false;
  if (debugarg) debug = atoi(debugarg);
  reset(debug);
}

/*!
  INIT <game> [<userid>]
 */
void init_action()
{
  char *gamearg = sCmd.next();
  if (!gamearg) {
    error("protocol error", "wrong # of parameters to INIT");
    return;
  }
  
  if (!strcmp(gamearg, HANDSHAKE_STR)) {
    global_state |= STATE_HANDSHAKE;
  }
  else if (!strcmp(gamearg, KNOCK_STR)) {
    global_state |= STATE_KNOCK;
    digitalWrite(LED2_PIN, 1);
  }
  else {
    if (global_debug) {
      Serial.print("DEBUG Unknown action ");
      Serial.println(gamearg);
    }
    return;
  }

  char *userarg = sCmd.next();
  if (userarg) {
    global_userid = getUserIdArg(userarg);
    // FIXME: Validity check?
  }

  if (global_debug) {
    Serial.print("DEBUG Initialized ");
    Serial.print(gamearg);
    Serial.print(", userid=");
    Serial.println(global_userid);
  }
}

/*!
  INIT <game>
*/
void exit_action()
{
  char *gamearg = sCmd.next();
  if (!strcmp(gamearg, HANDSHAKE_STR)) {
    global_state &= ~STATE_HANDSHAKE;
  }
  else if (!strcmp(gamearg, KNOCK_STR)) {
    global_state &= ~STATE_KNOCK;
    digitalWrite(LED2_PIN, 0);

    digitalWrite(BUZZER_PIN, 1);
    delay(2000);
    digitalWrite(BUZZER_PIN, 0);
  }
  else {
    if (global_debug) {
      Serial.print("DEBUG Unknown action ");
      Serial.println(gamearg);
    }
    return;
  }

  if (global_debug) {
    Serial.print("DEBUG Exited ");
    Serial.println(gamearg);
  }
}

/*!
  HANDSHAKE <bool> <userid>
*/
void handshake_action()
{
  char *activearg = sCmd.next();

  if (!activearg) {
    error("protocol error", "wrong # of parameters to HANDSHAKE");
    return;
  }

  bool active = atoi(activearg);

  char *userarg = sCmd.next();
  if (!userarg) {
    error("protocol error", "wrong # of parameters to HANDSHAKE");
    return;
  }
  int user = getUserIdArg(userarg);
  
  if (global_debug) {
    Serial.print("DEBUG External handshake from user ");
    Serial.println(user);
  }

  // FIXME: Provide physical handshake feedback
  // FIXME: Start/reset timers etc to fade out handshake indicators
}

/*!
  PATTERN <knock-array>

  Examples:
  No initial delay: knock-knock-knock--knock
  PATTERN 0 500 500 1000
  
  Initial delay: -knock-knock--knock
  PATTERN 500 500 1000
  
  Correct shave&haircut answer:
  PATTERN 1000 500

 */
void pattern_action()
{
  bool echo = false;
  char *arg = sCmd.next();
  if (!arg) {
    error("protocol error", "wrong # of parameters to PATTERN");
    return;
  }

  if (!strcmp(arg, "ECHO")) {
    echo = true;
    arg = sCmd.next();
  }

  knock_pattern_count = 0;
  while (arg) {
    knock_pattern[knock_pattern_count++] = atoi(arg);
    arg = sCmd.next();
  }

  if (global_debug) {
    Serial.print("DEBUG Knock received: ");
    for (uint8_t i=0;i<knock_pattern_count;i++) {
      Serial.print(knock_pattern[i]);
      Serial.print(" ");
    }
    Serial.println();
  }

  digitalWrite(LED1_PIN, 1);
  digitalWrite(LED2_PIN, 0);
  knocker.playPattern(knock_pattern, knock_pattern_count, echo ? 17 : 40);
  digitalWrite(LED1_PIN, 0);
  digitalWrite(LED2_PIN, 1);
}

// Send pattern over serial
void send_pattern(unsigned int *pattern, uint8_t length)
{
  Serial.print("PATTERN");
  for (uint8_t i=0;i<length;i++) {
    Serial.print(" ");
    Serial.print(pattern[i]);
  }
  Serial.println();
}

void setup ()
{ 
  Serial.begin(115200);

  sCmd.addCommand("RESET", reset_action);
  sCmd.addCommand("INIT", init_action);
  sCmd.addCommand("EXIT", exit_action);
  sCmd.addCommand("HANDSHAKE", handshake_action);
  sCmd.addCommand("PATTERN", pattern_action);
  sCmd.setDefaultHandler(unrecognized);

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
  global_state = STATE_IDLE;
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
    send_pattern(recorded_pattern, len);
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

void loop()
{
  sCmd.readSerial();

  if (global_state & STATE_KNOCK) handleKnock();
  if (global_state & STATE_HANDSHAKE) handleHandshake();

  // FIXME: Handle fading of handshake indicators
}
