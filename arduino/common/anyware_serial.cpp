#include "anyware_serial.h"

SerialCommand sCmd;

void addCommand(const char *cmd, void(*function)(), const char *wildcard)
{
  sCmd.addCommand(cmd, function);
  Serial.print(cmd);
  if (wildcard) Serial.print(wildcard);
  Serial.println();
}

void handleSerial()
{
  sCmd.readSerial();
}

int getUserIdArg(const char *arg)
{
  if (!strcmp(arg, "0")) return 0;
  else if (!strcmp(arg, "1")) return 1;
  else if (!strcmp(arg, "2")) return 2;
  else return -1;
}
