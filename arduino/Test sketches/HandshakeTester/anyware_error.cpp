#include "anyware_error.h"

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
