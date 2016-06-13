#include "SerialCommand.h"
#include "anyware_serial.h"

void unrecognized(const char *cmd)
{
  printError(F("unrecognized command"), cmd);
}

/*!
  HELLO [<bool>]
*/
int32_t lastHelloTime = -2*HELLO_BLACKOUT_TIME;
void hello_action()
{
  char *debugarg = sCmd.next();
  bool debug = false;
  if (debugarg) debug = atoi(debugarg);

  // Ignore duplicate HELLO messages within the blackout time period
  uint32_t ms = millis() - lastHelloTime;
  if (global_debug) {
    Serial.print(F("DEBUG HELLO received after "));
    Serial.print(ms);
    Serial.println(" ms");
  }
  if (ms < HELLO_BLACKOUT_TIME) {
    if (global_debug) Serial.println(F("DEBUG HELLO received during blackout"));
    return;
  }

  resetInterface(debug);

  lastHelloTime = millis();
}

void setupCommonCommands()
{ 
  sCmd.addCommand("HELLO", hello_action);
  sCmd.setDefaultHandler(unrecognized);
}
