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

const int KNOCK_STRENGTH = 40;
const int ECHO_STRENGTH = 17;
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

/*!
  RESET [<bool>]
*/
void reset_action()
{
  char *debugarg = sCmd.next();
  bool debug = false;
  if (debugarg) debug = atoi(debugarg);
  reset(debug);
}

/*!
  IDENTITY <userid>
*/
void identity_action()
{
  char *userarg = sCmd.next();
  if (!userarg) {
    error("protocol error", "wrong # of parameters to IDENTITY");
    return;
  }
  global_userid = getUserIdArg(userarg);
  // FIXME: Validity check?

  if (global_debug) {
    Serial.print("DEBUG Identity set, userid=");
    Serial.println(global_userid);
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
  KNOCK-INIT
 */
void knock_init_action()
{
  global_state |= STATE_KNOCK;
  digitalWrite(LED2_PIN, 1);

  if (global_debug) {
    Serial.println("DEBUG KNOCK-INIT received");
  }
}

/*!
  KNOCK-EXIT
*/
void knock_exit_action()
{
  global_state &= ~STATE_KNOCK;
  digitalWrite(LED2_PIN, 0);

  if (global_debug) {
    Serial.println("DEBUG KNOCK-EXIT received");
  }

  // FIXME: We simulate the transition for now
  digitalWrite(BUZZER_PIN, 1);
  delay(2000);
  digitalWrite(BUZZER_PIN, 0);
}

/*!
  KNOCK-PATTERN <knock-array>

  Examples:
  No initial delay: knock-knock-knock--knock
  PATTERN 0 500 500 1000
  
  Initial delay: -knock-knock--knock
  PATTERN 500 500 1000
  
  Correct shave&haircut answer:
  PATTERN 1000 500

 */
void knock_pattern_action()
{
  bool echo = false;
  char *arg;

  knock_pattern_count = 0;
  while (arg = sCmd.next()) {
    knock_pattern[knock_pattern_count++] = atoi(arg);
  }

  if (global_debug) {
    Serial.print("DEBUG KNOCK-PATTERN received: ");
    for (uint8_t i=0;i<knock_pattern_count;i++) {
      Serial.print(knock_pattern[i]);
      Serial.print(" ");
    }
    Serial.println();
  }

  digitalWrite(LED1_PIN, 1);
  digitalWrite(LED2_PIN, 0);
  knocker.playPattern(knock_pattern, knock_pattern_count, KNOCK_STRENGTH);
  digitalWrite(LED1_PIN, 0);
  digitalWrite(LED2_PIN, 1);
}

/*!
  KNOCK-ECHO <userid> <knock-array>

  Similar to KNOCK-PATTERN but with a userid as the first argument
*/
void knock_echo_action()
{
  bool echo = false;
  char *userarg = sCmd.next();
  if (!userarg) {
    error("protocol error", "wrong # of parameters to KNOCK-ECHO");
    return;
  }

  int user = getUserIdArg(userarg);
  // FIXME: Validity check?

  knock_pattern_count = 0;
  char *arg;
  while (arg = sCmd.next()) {
    knock_pattern[knock_pattern_count++] = atoi(arg);
  }

  if (global_debug) {
    Serial.print("DEBUG KNOCK_ECHO received: ");
    Serial.print(user);
    Serial.print(" ");
    for (uint8_t i=0;i<knock_pattern_count;i++) {
      Serial.print(knock_pattern[i]);
      Serial.print(" ");
    }
    Serial.println();
  }

  digitalWrite(LED1_PIN, 1);
  digitalWrite(LED2_PIN, 0);
  knocker.playPattern(knock_pattern, knock_pattern_count, ECHO_STRENGTH);
  digitalWrite(LED1_PIN, 0);
  digitalWrite(LED2_PIN, 1);
}

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

void setup ()
{ 
  Serial.begin(115200);

  sCmd.addCommand("RESET", reset_action);
  sCmd.addCommand("IDENTITY", identity_action);

  sCmd.addCommand("HANDSHAKE", handshake_action);

  sCmd.addCommand("KNOCK-INIT", knock_init_action);
  sCmd.addCommand("KNOCK-EXIT", knock_exit_action);
  sCmd.addCommand("KNOCK-PATTERN", knock_pattern_action);
  sCmd.addCommand("KNOCK-ECHO", knock_echo_action);
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

void loop()
{
  sCmd.readSerial();

  if (global_state & STATE_KNOCK) handleKnock();
  if (global_state & STATE_HANDSHAKE) handleHandshake();

  // FIXME: Handle fading of handshake indicators
}
