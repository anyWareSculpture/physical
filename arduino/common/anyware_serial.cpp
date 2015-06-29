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

void printError(const char *type, const char *msg)
{
  Serial.print(F("ERROR "));
  Serial.print(type);
  Serial.print(F(": "));
  Serial.println(msg);
}

void printError(const __FlashStringHelper *type, const __FlashStringHelper *msg)
{
  Serial.print(F("ERROR "));
  Serial.print(type);
  Serial.print(F(": "));
  Serial.println(msg);
}

void printError(const __FlashStringHelper *type, const char *msg)
{
  Serial.print(F("ERROR "));
  Serial.print(type);
  Serial.print(F(": "));
  Serial.println(msg);
}

int getUserIdArg(const char *arg)
{
  if (!strcmp(arg, "0")) return 0;
  else if (!strcmp(arg, "1")) return 1;
  else if (!strcmp(arg, "2")) return 2;
  else return -1;
}
