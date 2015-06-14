#include "Arduino.h"
#include "anyware_global.h"

int global_userid = -1;

// Convert separate R,G,B into packed 32-bit RGB color.
// Packed format is always RGB, regardless of LED strand color order.
uint32_t Color(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
}

void printError(const char *type, const char *msg)
{
  Serial.print("ERROR ");
  Serial.print(type);
  Serial.print(": ");
  Serial.println(msg);
}

#define USER0_STR "0"
#define USER1_STR "1"
#define USER2_STR "2"

int getUserIdArg(const char *arg)
{
  if (!strcmp(arg, USER0_STR)) return 0;
  else if (!strcmp(arg, USER1_STR)) return 1;
  else if (!strcmp(arg, USER2_STR)) return 2;
  else return -1;
}
