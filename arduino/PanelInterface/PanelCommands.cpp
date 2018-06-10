#include "PanelInterface.h"
#include "anyware_serial.h"
#include "anyware_colors.h"
#include "PanelSet.h"

#include "configuration.h"
#include "Controllers.h"

extern SerialCommand sCmd;

/*!
  PANEL-SET <strip> <panel> <intensity> <color> <easing>
*/
void panel_set_action()
{
  char *striparg = sCmd.next();
  if (!striparg) {
    printError(F("protocol error"), F("wrong # of parameters to PANEL-SET"));
    return;
  }
  uint8_t strip = atoi(striparg);
  if (strip >= MAX_STRIPS) {
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
    if (digit < '0' || digit > '9') {
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
    Serial.println();
  }
  do_panel_set(strip, panels, intensity, color, easing);
}

/*!
  PANEL-PULSE <strip> <panel> <intensity> <color> <easing>
*/
void panel_pulse_action()
{
  char *striparg = sCmd.next();
  if (!striparg) {
    printError(F("protocol error"), F("wrong # of parameters to PANEL-PULSE"));
    return;
  }
  uint8_t strip = atoi(striparg);
  if (strip >= MAX_STRIPS) {
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
    if (digit < '0' || digit > '9') {
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
  AnywareEasing::EasingType easing = AnywareEasing::BINARY_PULSE;
  if (easingarg && strcmp(easingarg, "-")) {
    if (!AnywareEasing::getEasing(easingarg, easing)) {
      printError(F("protocol error"), F("Illegal easing argument"));
      printError(F("protocol error"), easingarg);
      return;
    }
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
    Serial.println();
  }
  do_panel_pulse(strip, panels, intensity, color, easing);
}

/*!
  PANEL-INTENSITY <strip> <intensity>
*/
void panel_intensity_action()
{
  char *striparg = sCmd.next();
  if (!striparg) {
    printError(F("protocol error"), F("wrong # of parameters to PANEL-PULSE"));
    return;
  }
  uint8_t strip = atoi(striparg);
  if (strip = MAX_STRIPS) {
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

void setupCommands()
{ 
  setupCommonCommands();

  addCommand("PANEL-SET", panel_set_action);
  addCommand("PANEL-PULSE", panel_pulse_action);
  addCommand("PANEL-INTENSITY", panel_intensity_action);
}

void printCommands()
{ 
  Serial.println(F("SUPPORTED"));
  printCommand("PANEL-SET", STRIPREGEXP(STRIPID));
  printCommand("PANEL-PULSE", STRIPREGEXP(STRIPID));
  printCommand("PANEL-INTENSITY", STRIPREGEXP(STRIPID));
  Serial.println(F("ENDSUPPORTED"));
}
