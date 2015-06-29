#include "SerialCommand.h"
#include "DiskGame.h"

extern SerialCommand sCmd;

/*!
  DISK-INIT

  Initialize disk game. This just allows the Arduino to listen to DISK commands.
 */
void disk_init_action()
{
  if (global_userid < 0) {
    printError("protocol error", "DISK-INIT received without IDENTITY");
    return;
  }

  if (global_debug) {
    Serial.println("DEBUG DISK-INIT received");
  }

  do_disk_init();
}

/*!
  DISK-EXIT

  Exit disk game. Stop any currently running animation or process.
  No longer listen to DISK commands.
*/
void disk_exit_action()
{
  if (global_debug) {
    Serial.println("DEBUG DISK-EXIT received");
  }

  do_disk_exit();
}

/*!
  DISK-RESET

  Resets the game, home the disks and afterwards to into "ready" mode.
*/
void disk_reset_action()
{
  if (!(global_state & STATE_DISK)) return;

  if (global_debug) {
    Serial.println("DEBUG DISK-RESET received");
  }

  do_disk_reset();
}


/*!
  DISK-STATE <state>  (state = "success")

  Only supports the "success" state. Show success light pattern/animation.
 */
void disk_state_action()
{
  if (!(global_state & STATE_DISK)) return;

  char *arg = sCmd.next();
  if (!strcmp(arg, "success")) {
    do_disk_state();
  }
  else {
    if (global_debug) {
      Serial.print("DEBUG Unknown state ");
      Serial.println(arg);
    }
    return;
  }

  if (global_debug) {
    Serial.print("DEBUG New state: ");
    Serial.println(arg);
  }
}

/*!
  DISK <diskid> POS <deg> DIR <dir> USER <userid>
*/
void disk_action()
{
  if (!(global_state & STATE_DISK)) return;

  char *arg = sCmd.next();
  uint8_t diskid = atoi(arg);
  // FIXME: Validate disk ID
  DirectionFlags dir = DIR_OFF;
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
        case DIR_OFF:
        case DIR_CCW:
        case DIR_CW:
        case DIR_BLOCKED:
          dir = (DirectionFlags)dirarg;
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

  do_disk(diskid, userid, pos, dir);
}

void setupCommands()
{ 
  setupCommonCommands();

  sCmd.addCommand("DISK-INIT", disk_init_action);
  sCmd.addCommand("DISK-RESET", disk_reset_action);
  sCmd.addCommand("DISK-STATE", disk_state_action);
  sCmd.addCommand("DISK", disk_action);
  sCmd.addCommand("DISK-EXIT", disk_exit_action);
}
