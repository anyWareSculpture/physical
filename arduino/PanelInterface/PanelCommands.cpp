#include "SerialCommand.h"
#include "anyware_global.h"
#include "PanelInterface.h"

extern SerialCommand sCmd;

/*!
  PANEL-INIT
 */
void panel_init_action()
{
  if (global_userid < 0) {
    printError(F("protocol error"), F("DISK-INIT received without IDENTITY"));
    return;
  }

  if (global_debug) Serial.println(F("DEBUG PANEL-INIT received"));
  do_panel_init();
}

/*!
  PANEL-EXIT
*/
void panel_exit_action()
{
  if (global_debug) Serial.println(F("DEBUG PANEL-EXIT received"));
  do_panel_exit();
}

bool getColor(const char *colorstr, uint32_t &col)
{
  if (!strcmp(colorstr, "user0")) {
    col = locationColor[0];
  }
  else if (!strcmp(colorstr, "user1")) {
    col = locationColor[1];
  }
  else if (!strcmp(colorstr, "user2")) {
    col = locationColor[2];
  }
  else if (!strcmp(colorstr, "error")) {
    col = RED;
  }
  else if (!strcmp(colorstr, "success")) {
    col = GREEN;
  }
  else if (!strcmp(colorstr, "black")) {
    col = BLACK;
  }
  else if (!strcmp(colorstr, "white")) {
    col = WHITE;
  }
  else {
    return false;
  }
  return true;
}

bool getEasing(const char *easingstr, AnywareEasing::EasingType &easing)
{
  if (!strcmp(easingstr, "easein")) {
    easing = AnywareEasing::IN_CUBIC;
  }
  else if (!strcmp(easingstr, "pulse")) {
    easing = AnywareEasing::PULSE;
  }
  else if (!strcmp(easingstr, "pop")) {
    easing = AnywareEasing::POP;
  }
  else {
    return false;
  }

  return true;
}

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
  if (strip > 2) {
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
  uint32_t color;
  if (colorarg && strcmp(colorarg, "-")) {
    if (!getColor(colorarg, color)) {
      printError(F("protocol error"), F("Illegal color argument"));
      return;
    }
  }

  char *easingarg = sCmd.next();
  AnywareEasing::EasingType easing = AnywareEasing::BINARY;
  if (easingarg && strcmp(easingarg, "-")) {
    if (!getEasing(easingarg, easing)) {
      printError(F("protocol error"), F("Illegal easing argument"));
      return;
    }
  }

  if (global_debug) {
    Serial.print(F("DEBUG PANEL-SET received: "));
    Serial.print(easing);
    // FIXME: output
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
  if (strip > 2) {
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
  uint32_t color;
  if (colorarg && strcmp(colorarg, "-")) {
    if (!getColor(colorarg, color)) {
      printError(F("protocol error"), F("Illegal color argument"));
      return;
    }
  }

  char *easingarg = sCmd.next();
  AnywareEasing::EasingType easing = AnywareEasing::BINARY;
  if (easingarg && strcmp(easingarg, "-")) {
    if (!getEasing(easingarg, easing)) {
      printError(F("protocol error"), F("Illegal easing argument"));
      return;
    }
  }

  if (global_debug) {
    Serial.print(F("DEBUG PANEL-PULSE received: "));
    // FIXME: output
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
  if (strip > 2) {
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

  sCmd.addCommand("PANEL-INIT", panel_init_action);
  sCmd.addCommand("PANEL-EXIT", panel_exit_action);
  sCmd.addCommand("PANEL-SET", panel_set_action);
  sCmd.addCommand("PANEL-PULSE", panel_pulse_action);
  sCmd.addCommand("PANEL-INTENSITY", panel_intensity_action);
  sCmd.addCommand("PANEL-ANIMATE", panel_animate_action);
  sCmd.addCommand("PANEL-STATE", panel_state_action);
}
