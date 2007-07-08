
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

void    send_patch     		(struct fsex_libdata *, int);
int	recv_patch		(struct fsex_libdata *, int, int, uint8 *);
uint8	*sysex_get_id		(int);
void	show_id			(uint8 *);
int	map_synth_patches	(char *, struct fsex_libdata *);
void	get_patches		(int, char **, struct fsex_libdata *,
				 int, char *);
#endif
