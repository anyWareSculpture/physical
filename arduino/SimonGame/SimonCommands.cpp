#include "SerialCommand.h"
#include "anyware_global.h"
#include "SimonGame.h"

extern SerialCommand sCmd;

/*!
  SIMON-INIT
 */
void simon_init_action()
{
  if (global_userid < 0) {
    printError("protocol error", "DISK-INIT received without IDENTITY");
    return;
  }

  if (global_debug) Serial.println("DEBUG SIMON-INIT received");
  do_simon_init();
}

/*!
  SIMON-EXIT
*/
void simon_exit_action()
{
  if (global_debug) Serial.println("DEBUG SIMON-EXIT received");
  do_simon_exit();
}

/*!
  SIMON-PATTERN <pattern>
*/
uint8_t simon_pattern[MAX_SIMON_PATTERN];
uint8_t simon_pattern_count;
void simon_pattern_action()
{
  char *arg;

  simon_pattern_count = 0;
  while ((arg = sCmd.next()) && simon_pattern_count < MAX_SIMON_PATTERN) {
    simon_pattern[simon_pattern_count++] = atoi(arg);
  }

  if (global_debug) {
    Serial.print("DEBUG SIMON-PATTERN received: ");
    for (uint8_t i=0;i<simon_pattern_count;i++) {
      Serial.print(simon_pattern[i]);
      Serial.print(" ");
    }
    Serial.println();
  }
  do_simon_pattern(simon_pattern, simon_pattern_count);
}

/*!
  SIMON-SENSORS <userid> <sensors>
*/
void simon_sensors_action()
{
  char *userarg = sCmd.next();
  if (!userarg) {
    printError("protocol error", "wrong # of parameters to KNOCK-ECHO");
    return;
  }

  int userid = getUserIdArg(userarg);
  if (userid < 0 || userid > 2) {
    printError("protocol error", "Illegal USER argument");
    return;
  }

  // remoteSensors is a global array
  char *arg;
  for (uint8_t i=0;i<NUMPIXELS;i++) {
    bool val = false;
    if (arg = sCmd.next()) val = atoi(arg);
    remoteSensors[i] = val;
  }

  if (global_debug) {
    Serial.print("DEBUG SIMON-SENSORS received: ");
    Serial.print(userid);
    Serial.print(" ");
    for (uint8_t i=0;i<NUMPIXELS;i++) {
      Serial.print(remoteSensors[i]);
      Serial.print(" ");
    }
    Serial.println();
  }
  do_simon_sensors(userid);
}

/*!
  SIMON-STATE <"success" | "failure">
*/
void simon_state_action()
{
  char *arg = sCmd.next();
  int state;
  if (!strcmp(arg, "success")) {
    state = STATE_SUCCESS;
  }
  else if (!strcmp(arg, "failure")) {
    state = STATE_FAILURE;
  }
  else {
    printError("protocol error", "Illegal argument to SIMON-STATE");
    return;
  }

  if (global_debug) {
    Serial.print("DEBUG SIMON-STATE received: ");
    Serial.println(arg);
  }
  do_simon_state(state);
}

void setupCommands()
{ 
  setupCommonCommands();

  sCmd.addCommand("SIMON-INIT", simon_init_action);
  sCmd.addCommand("SIMON-EXIT", simon_exit_action);
  sCmd.addCommand("SIMON-PATTERN", simon_pattern_action);
  sCmd.addCommand("SIMON-SENSORS", simon_sensors_action);
  sCmd.addCommand("SIMON-STATE", simon_state_action);
}
