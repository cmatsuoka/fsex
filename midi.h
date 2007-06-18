
#ifndef __MIDI_H
#define __MIDI_H

int midi_open		(int, int);
void midi_close		(void);
void midi_noteon	(int, int, int);
void midi_noteoff	(char, int, int);
void midi_bank		(int, int, int);
void midi_pgm		(int, int);
void midi_ctl		(int, int, int);
void midi_bend		(int, int);
void midi_sysex		(int, void *);

#endif
