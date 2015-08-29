#ifndef ANYWARE_ERROR_H_
#define ANYWARE_ERROR_H_

#include <Arduino.h>
#include <avr/pgmspace.h>

void printError(const char *type, const char *msg);
void printError(const __FlashStringHelper *type, const __FlashStringHelper *msg);
void printError(const __FlashStringHelper *type, const char *msg);

#endif
