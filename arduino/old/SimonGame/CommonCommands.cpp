#include "SerialCommand.h"
#include "anyware_global.h"

// Set to 1 to start in debug mode
#define DEBUG_MODE 1

SerialCommand sCmd;

void handleSerial()
{
  sCmd.readSerial();
}

void unrecognized(const char *cmd)
{
  printError("unrecognized command", cmd);
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
    printError("protocol error", "wrong # of parameters to IDENTITY");
    return;
  }
  global_userid = getUserIdArg(userarg);
  // FIXME: Validity check?

  if (global_debug) {
    Serial.print("DEBUG Identity set, userid=");
    Serial.println(global_userid);
  }
}

void setupCommonCommands()
{ 
  sCmd.addCommand("RESET", reset_action);
  sCmd.addCommand("IDENTITY", identity_action);
  sCmd.setDefaultHandler(unrecognized);
}
