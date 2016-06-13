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

void setupCommands()
{ 
  setupCommonCommands();
  addCommand("DISK-RESET", disk_reset_action);
  addCommand("DISK", disk_action);
}

void printCommands()
{ 
  Serial.println(F("SUPPORTED"));
  printCommand("DISK-RESET");
  printCommand("DISK");
  Serial.println(F("ENDSUPPORTED"));
}
