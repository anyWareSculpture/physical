#include "HandshakeInterface.h"
#include "anyware_serial.h"
#include "PanelSet.h"

/*!
  PANEL-SET <strip> <panel> <intensity> <color> <easing> <duration>

  Listens for strip 3, 5 and 6
*/
void panel_set_action()
{
  char *striparg = sCmd.next();
  if (!striparg) {
    printError(F("protocol error"), F("wrong # of parameters to PANEL-SET"));
    return;
  }
  uint8_t strip = atoi(striparg);
  if (strip != 3 && strip != 5 && strip != 6) {
    printError(F("protocol error"), F("Illegal strip argument"));
    return;
  }

  char *panelarg = sCmd.next();
  if (!panelarg) {
    printError(F("protocol error"), F("wrong # of parameters to PANEL-SET"));
    return;
  }
  PanelSet panels;
  const uint8_t numPanels = strlen(panelarg);
  for (uint8_t i=0;i<numPanels;++i) {
    const char digit = panelarg[i];
    if (digit < '0' ||
	strip == 3 && digit >= '2' ||
	strip == 5 && digit >= '4' ||
	strip == 6 && digit >= '3') {
      printError(F("protocol error"), F("Illegal panel argument"));
      return;
    }
    uint8_t panel = digit - '0';
    panels.set(panel);
  }

  char *intensityarg = sCmd.next();
  if (!intensityarg) {
    printError(F("protocol error"), F("wrong # of parameters to PANEL-SET"));
    return;
  }
  uint8_t intensity = atoi(intensityarg);
  if (intensity > 100) {
    printError(F("protocol error"), F("Illegal intensity argument"));
    return;
  }

  char *colorarg = sCmd.next();
  CRGB color;
  if (colorarg && strcmp(colorarg, "-")) {
    if (!getColor(colorarg, color)) {
      printError(F("protocol error"), F("Illegal color argument"));
      return;
    }
  }

  char *easingarg = sCmd.next();
  AnywareEasing::EasingType easing = AnywareEasing::BINARY;
  if (easingarg && strcmp(easingarg, "-")) {
    if (!AnywareEasing::getEasing(easingarg, easing)) {
      printError(F("protocol error"), F("Illegal easing argument"));
      return;
    }
  }

  char *durationarg = sCmd.next();
  uint16_t duration = 0;
  if (durationarg) {
    duration = atoi(durationarg);
  }

  if (global_debug) {
    Serial.print(F("DEBUG PANEL-SET received: "));
    Serial.print(strip);
    Serial.print(" ");
    Serial.print(panels.panels);
    Serial.print(" ");
    Serial.print(intensity);
    Serial.print(" ");
    Serial.print(color, HEX);
    Serial.print(" ");
    Serial.print(easing);
    Serial.print(" ");
    Serial.print(duration);
    Serial.println();
  }
  do_panel_set(strip, panels, intensity, color, easing, duration);
}

/*!
  PANEL-PULSE <strip> <panel> <intensity> <color> <easing> <duration>

  Listens for strip 3, 5 and 6
*/
void panel_pulse_action()
{
  char *striparg = sCmd.next();
  if (!striparg) {
    printError(F("protocol error"), F("wrong # of parameters to PANEL-PULSE"));
    return;
  }
  uint8_t strip = atoi(striparg);
  if (strip != 3 && strip != 5 && strip != 6) {
    printError(F("protocol error"), F("Illegal strip argument"));
    return;
  }

  char *panelarg = sCmd.next();
  if (!panelarg) {
    printError(F("protocol error"), F("wrong # of parameters to PANEL-PULSE"));
    return;
  }
  PanelSet panels;
  const uint8_t numPanels = strlen(panelarg);
  for (uint8_t i=0;i<numPanels;++i) {
    const char digit = panelarg[i];
    if (digit < '0' ||
	strip == 3 && digit >= '2' ||
	strip == 5 && digit >= '4' ||
	strip == 6 && digit >= '3') {
      printError(F("protocol error"), F("Illegal panel argument"));
      return;
    }
    uint8_t panel = digit - '0';
    panels.set(panel);
  }

  char *intensityarg = sCmd.next();
  if (!intensityarg) {
    printError(F("protocol error"), F("wrong # of parameters to PANEL-PULSE"));
    return;
  }
  uint8_t intensity = atoi(intensityarg);
  if (intensity > 100) {
    printError(F("protocol error"), F("Illegal intensity argument"));
    return;
  }

  char *colorarg = sCmd.next();
  CRGB color;
  if (colorarg && strcmp(colorarg, "-")) {
    if (!getColor(colorarg, color)) {
      printError(F("protocol error"), F("Illegal color argument"));
      return;
    }
  }

  char *easingarg = sCmd.next();
  AnywareEasing::EasingType easing = AnywareEasing::BINARY;
  if (easingarg && strcmp(easingarg, "-")) {
    if (!AnywareEasing::getEasing(easingarg, easing)) {
      printError(F("protocol error"), F("Illegal easing argument"));
      return;
    }
  }

  char *durationarg = sCmd.next();
  uint16_t duration = 0;
  if (durationarg) {
    duration = atoi(durationarg);
  }

  if (global_debug) {
    Serial.print(F("DEBUG PANEL-PULSE received: "));
    Serial.print(strip);
    Serial.print(" ");
    Serial.print(panels.panels);
    Serial.print(" ");
    Serial.print(intensity);
    Serial.print(" ");
    Serial.print(color, HEX);
    Serial.print(" ");
    Serial.print(easing);
    Serial.print(" ");
    Serial.print(duration);
    Serial.println();
  }
  do_panel_pulse(strip, panels, intensity, color, easing, duration);
}

/*!
  PANEL-INTENSITY <strip> <intensity>

  Listens for strip 3, 5 and 6
*/
void panel_intensity_action()
{
  char *striparg = sCmd.next();
  if (!striparg) {
    printError(F("protocol error"), F("wrong # of parameters to PANEL-PULSE"));
    return;
  }
  uint8_t strip = atoi(striparg);
  if (strip != 3 && strip != 5 && strip != 6) {
    printError(F("protocol error"), F("Illegal strip argument"));
    return;
  }

  char *intensityarg = sCmd.next();
  if (!intensityarg) {
    printError(F("protocol error"), F("wrong # of parameters to PANEL-PULSE"));
    return;
  }
  uint8_t intensity = atoi(intensityarg);
  if (intensity > 100) {
    printError(F("protocol error"), F("Illegal intensity argument"));
    return;
  }

  do_panel_intensity(strip, intensity);
}

/*!
  HANDSHAKE <numusers> (0-3)
*/
void handshake_action()
{
  char *usersarg = sCmd.next();
  if (!usersarg) {
    printError(F("protocol error"), F("wrong # of parameters to HANDSHAKE"));
    return;
  }
  uint8_t numusers = atoi(usersarg);
  if (numusers > 3) {
    printError(F("protocol error"), F("Illegal numusers argument"));
    return;
  }

  do_handshake(numusers);
}

void setupCommands()
{ 
  setupCommonCommands();
  addCommand("PANEL-SET", panel_set_action);
  addCommand("PANEL-PULSE", panel_pulse_action);
  addCommand("PANEL-INTENSITY", panel_intensity_action);
  addCommand("HANDSHAKE", handshake_action);
}

void printCommands()
{
  Serial.println(F("SUPPORTED"));
  printCommand("PANEL-SET", " [356]");
  printCommand("PANEL-PULSE", " [356]");
  printCommand("PANEL-INTENSITY", " [356]");
  printCommand("HANDSHAKE");
  Serial.println(F("ENDSUPPORTED"));
}
