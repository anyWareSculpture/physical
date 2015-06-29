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
  reset(debug);
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
  global_userid = getUserIdArg(userarg);
  // FIXME: Validity check?

  if (global_debug) {
    Serial.print(F("DEBUG Identity set, userid="));
    Serial.println(global_userid);
  }
}

void setupCommonCommands()
{ 
  sCmd.addCommand("RESET", reset_action);
  sCmd.addCommand("IDENTITY", identity_action);
  sCmd.setDefaultHandler(unrecognized);
}
