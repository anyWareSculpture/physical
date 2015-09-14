#ifndef SIMONGAME_H_
#define SIMONGAME_H_

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      10

// Max pattern length
#define MAX_SIMON_PATTERN 20

// State flags
#define STATE_IDLE      0x00
#define STATE_PLAYBACK  0x01
#define STATE_READY     0x02
#define STATE_SUCCESS   0x03
#define STATE_FAILURE   0x04

extern int global_state;
extern bool remoteSensors[NUMPIXELS];

void do_simon_init();
void do_simon_exit();
void do_simon_pattern(uint8_t *simon_pattern, uint8_t simon_pattern_count);
void do_simon_sensors(uint8_t userid);
void do_simon_state(int state);

#endif
