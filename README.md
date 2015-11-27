# physical
Physical computing aspects of anyWare (firmware, electronics)

## arduino

The Arduino sketched share some common code.
However, there are no good mechanism for sharing code when using the IDE.

Our workaround for now is to keep shared code in the common/ folder
and copy the files from there into each sketch folder.
Not nice, but the result is self-contained sketches.

We use the Teensy 3.x for parts of the system. Download the Teensyduino add-on from here: https://www.pjrc.com/teensy/td_download.html
Note: We don't need any libraries installed with Teensy as we include our own.

Note: As of December 2015, our code doesn't compile with Arduino >= 1.6.6.

### Test sketches

These are just for hardware testing

### Test games

These are basic self-contained game sketches, for playtesting in isolation

