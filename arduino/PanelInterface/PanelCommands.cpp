#include "PanelInterface.h"
#include "anyware_serial.h"
#include "anyware_colors.h"
#include "configuration.h"

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
  uint8_t panel = atoi(panelarg);
  if (panel >= 10) {
    printError(F("protocol error"), F("Illegal panel argument"));
    return;
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
    Serial.print(panel);
    Serial.print(" ");
    Serial.print(intensity);
    Serial.print(" ");
    Serial.print(color, HEX);
    Serial.print(" ");
    Serial.print(easing);
    Serial.println();
  }
  do_panel_set(strip, panel, intensity, color, easing);
}

/*!
  PANEL-SENSORS <userid> <sensors>
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
  uint8_t panel = atoi(panelarg);
  if (panel >= 10) {
    printError(F("protocol error"), F("Illegal panel argument"));
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
    Serial.print(panel);
    Serial.print(" ");
    Serial.print(intensity);
    Serial.print(" ");
    Serial.print(color, HEX);
    Serial.print(" ");
    Serial.print(easing);
    Serial.println();
  }
  do_panel_pulse(strip, panel, intensity, color, easing);
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

/*!
  PANEL-ANIMATE <name>
*/
void panel_animate_action()
{
  Serial.println(F("PANEL-STATE animate"));
  Serial.println(F("PANEL-STATE ready"));
}

/*!
  PANEL-STATE <"success" | "failure">
*/
void panel_state_action()
{
  char *arg = sCmd.next();
  int state;
  if (!strcmp(arg, "success")) {
    state = STATE_SUCCESS;
  }
  else if (!strcmp(arg, "failure")) {
    state = STATE_FAILURE;
  }
  else {
    printError(F("protocol error"), F("Illegal argument to PANEL-STATE"));
    return;
  }

  if (global_debug) {
    Serial.print(F("DEBUG PANEL-STATE received: "));
    Serial.println(arg);
  }
  do_panel_state(state);
}

void setupCommands()
{ 
  setupCommonCommands();

  Serial.println(F("SUPPORTED"));
  addCommand("PANEL-SET", panel_set_action, STRIPREGEXP);
  addCommand("PANEL-PULSE", panel_pulse_action, STRIPREGEXP);
  addCommand("PANEL-INTENSITY", panel_intensity_action, STRIPREGEXP);
  addCommand("PANEL-ANIMATE", panel_animate_action);
  addCommand("PANEL-STATE", panel_state_action);
  Serial.println(F("ENDSUPPORTED"));
}
