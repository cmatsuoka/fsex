
#ifndef __FSEX_MIDI_H
#define __FSEX_MIDI_H

int midi_open		(char *, char *);
void midi_close		(void);
void midi_noteon	(int, int, int);
void midi_noteoff	(char, int, int);
void midi_bank		(int, int, int);
void midi_pgm		(int, int);
void midi_ctl		(int, int, int);
void midi_bend		(int, int);
void midi_sysex_send	(int, void *);
int midi_sysex_recv	(int, void *);

#endif
