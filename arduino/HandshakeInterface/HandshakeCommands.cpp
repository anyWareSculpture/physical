#include "SerialCommand.h"
#include "HandshakeInterface.h"

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
    Serial.print("DEBUG HANDSHAKE ");
    Serial.print(active);
    Serial.print(" ");
    Serial.print(user);
    Serial.println(" received");
  }
  do_handshake(active, user);
}

void setupCommands()
{ 
  setupCommonCommands();

  sCmd.addCommand("HANDSHAKE", handshake_action);
}
