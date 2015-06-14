#include "SerialCommand.h"
#include "KnockGame.h"

extern SerialCommand sCmd;

/*!
  HANDSHAKE <bool> <userid>
*/
void handshake_action()
{
  char *activearg = sCmd.next();

  if (!activearg) {
    printError("protocol error", "wrong # of parameters to HANDSHAKE");
    return;
  }

  bool active = atoi(activearg);

  char *userarg = sCmd.next();
  if (!userarg) {
    printError("protocol error", "wrong # of parameters to HANDSHAKE");
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
  if (global_userid < 0) {
    printError("protocol error", "DISK-INIT received without IDENTITY");
    return;
  }

  if (global_debug) {
    Serial.println("DEBUG KNOCK-INIT received");
  }
  do_knock_init();
}

/*!
  KNOCK-EXIT
*/
void knock_exit_action()
{
  if (global_debug) {
    Serial.println("DEBUG KNOCK-EXIT received");
  }
  do_knock_exit();
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
  char *arg;

  // knock_pattern and knock_pattern_count are global vars for now
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

  do_knock_pattern();
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
    printError("protocol error", "wrong # of parameters to KNOCK-ECHO");
    return;
  }

  int userid = getUserIdArg(userarg);
  if (userid < 0 || userid > 2) {
    printError("protocol error", "Illegal IDENTITY argument");
    return;
  }

  // knock_pattern and knock_pattern_count are global vars for now
  knock_pattern_count = 0;
  char *arg;
  while (arg = sCmd.next()) {
    knock_pattern[knock_pattern_count++] = atoi(arg);
  }

  if (global_debug) {
    Serial.print("DEBUG KNOCK_ECHO received: ");
    Serial.print(userid);
    Serial.print(" ");
    for (uint8_t i=0;i<knock_pattern_count;i++) {
      Serial.print(knock_pattern[i]);
      Serial.print(" ");
    }
    Serial.println();
  }

  do_knock_echo();
}

void setupCommands()
{ 
  setupCommonCommands();

  sCmd.addCommand("HANDSHAKE", handshake_action);

  sCmd.addCommand("KNOCK-INIT", knock_init_action);
  sCmd.addCommand("KNOCK-EXIT", knock_exit_action);
  sCmd.addCommand("KNOCK-PATTERN", knock_pattern_action);
  sCmd.addCommand("KNOCK-ECHO", knock_echo_action);
}
