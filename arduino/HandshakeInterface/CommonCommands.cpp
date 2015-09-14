#include "SerialCommand.h"
#include "anyware_serial.h"

void unrecognized(const char *cmd)
{
  printError(F("unrecognized command"), cmd);
}

/*!
  RESET [<bool>]
*/
void reset_action()
{
  char *debugarg = sCmd.next();
  bool debug = false;
  if (debugarg) debug = atoi(debugarg);
  resetInterface(debug);
}

/*!
  IDENTITY <userid>
*/
void identity_action()
{
  char *userarg = sCmd.next();
  if (!userarg) {
    printError(F("protocol error"), F("wrong # of parameters to IDENTITY"));
    return;
  }
  uint8_t userid = getUserIdArg(userarg);
  if (userid > 2) {
    printError(F("protocol error"), F("Illegal userid argument"));
    return;
  }
  global_userid = userid;

  if (global_debug) {
    Serial.print(F("DEBUG Identity set, userid="));
    Serial.println(global_userid);
  }
}

/*!
  INIT
*/
void init_action()
{
  global_initialized = true;
  if (global_debug) {
    Serial.println(F("DEBUG INIT received"));
  }
  initInterface();
}

void setupCommonCommands()
{ 
  sCmd.addCommand("RESET", reset_action);
  sCmd.addCommand("IDENTITY", identity_action);
  sCmd.addCommand("INIT", init_action);
  sCmd.setDefaultHandler(unrecognized);
}
