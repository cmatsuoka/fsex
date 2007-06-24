
#ifndef __FSEX_SEX_H
#define __FSEX_SEX_H

#include "library.h"

struct bank_sel {
	char *name;
	int min;
	int max;
	uint8 msb;
	uint8 lsb;
	int prg_bias;
};

void	send_sysex	(int, uint32, int, uint8 *);
void	recv_sysex	(int, uint32, int, int, uint8 *);
void    send_patch      (struct fsex_libdata *, int);
int	recv_patch	(char *, int, int, uint8 *);
int	checksum	(int, uint8 *);
void	sysex_get_id	(int);

#endif
