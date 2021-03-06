#include "DiskInterface.h"
#include "anyware_serial.h"

/*!
  DISK-RESET

  Resets the game, home the disks and afterwards to into "ready" mode.
*/
void disk_reset_action()
{
  if (global_debug) {
    Serial.println("DEBUG DISK-RESET received");
  }

  do_disk_reset();
}


/*!
  DISK <diskid> POS <deg> DIR <dir> USER <userid>
  // FIXME: Return error if missing POS/DIR/USER to avoid "null" commands
*/
void disk_action()
{
  char *arg = sCmd.next();
  uint8_t diskid = atoi(arg);
  if (diskid > 2) {
    printError(F("protocol error"), F("Illegal diskid argument"));
    return;
  }
  Direction dir = DIR_OFF;
  int pos = -1, userid = -1;
  while (arg = sCmd.next()) {
    if (!strcmp(arg, "POS")) {
      arg = sCmd.next();
      if (arg) {
        pos = atoi(arg);
      }
    }
    if (!strcmp(arg, "DIR")) {
      arg = sCmd.next();
      if (arg) {
        int dirarg = atoi(arg);
        switch (dirarg) {
        case DIR_CW:
        case DIR_OFF:
        case DIR_CCW:
          dir = (Direction)dirarg;
          break;
        default:
          printError("protocol error", "Illegal DIR argument");
          break;
        }
      }
    }
    if (!strcmp(arg, "USER")) {
      arg = sCmd.next();
      if (arg) {
        userid = atoi(arg);
        if (userid < 0 || userid > 2) {
          printError("protocol error", "Illegal USER argument");
          userid = 0;
        }
      }
    }
  }
  if (global_debug) {
    Serial.print(F("DEBUG DISK received: "));
    Serial.print(diskid);
    Serial.print(F(" POS "));
    Serial.print(pos);
    Serial.print(F(" DIR "));
    Serial.print(dir);
    Serial.println();
  }
  do_disk(diskid, userid, pos, dir);
}

/*!
  PANEL-SET <strip> <panel> <intensity> <color> <easing>

  Only listens for strip 3 and 4
*/
void panel_set_action()
{
  char *striparg = sCmd.next();
  if (!striparg) {
    printError(F("protocol error"), F("wrong # of parameters to PANEL-SET"));
    return;
  }
  uint8_t strip = atoi(striparg);
  if (strip < 3 || strip > 4) {
    printError(F("protocol error"), F("Illegal strip argument"));
    return;
  }

  char *panelarg = sCmd.next();
  if (!panelarg) {
    printError(F("protocol error"), F("wrong # of parameters to PANEL-SET"));
    return;
  }
  uint8_t panel = atoi(panelarg);
  if (strip == 4 && panel >= 3 ||
      strip == 3 && panel >= 6) {
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
  if (strip < 3 || strip > 4) {
    printError(F("protocol error"), F("Illegal strip argument"));
    return;
  }

  char *panelarg = sCmd.next();
  if (!panelarg) {
    printError(F("protocol error"), F("wrong # of parameters to PANEL-PULSE"));
    return;
  }
  uint8_t panel = atoi(panelarg);
  if (strip == 4 && panel >= 3 ||
      strip == 3 && panel >= 6) {
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
  AnywareEasing::EasingType easing = AnywareEasing::BINARY;
  if (easingarg && strcmp(easingarg, "-")) {
    if (!AnywareEasing::getEasing(easingarg, easing)) {
      printError(F("protocol error"), F("Illegal easing argument"));
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
  if (strip < 3 || strip > 4) {
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
  addCommand("DISK-RESET", disk_reset_action);
  addCommand("DISK", disk_action);
  addCommand("PANEL-SET", panel_set_action);
  addCommand("PANEL-PULSE", panel_pulse_action);
  addCommand("PANEL-INTENSITY", panel_intensity_action);
}

void printCommands()
{ 
  Serial.println(F("SUPPORTED"));
  printCommand("DISK-RESET");
  printCommand("DISK");
  printCommand("PANEL-SET", " [34]");
  printCommand("PANEL-PULSE", " [34]");
  printCommand("PANEL-INTENSITY", " [34]");
  Serial.println(F("ENDSUPPORTED"));
}
