
#ifndef __FSEX_SEX_H
#define __FSEX_SEX_H

#include "library.h"

void	send_sysex	(int, uint32, int, uint8 *);
void	recv_sysex	(int, uint32, int, uint8 *);
void    send_patch      (struct fsex_libdata *, int, int);
int	checksum	(int, uint8 *);
void	sysex_get_id	(int);

#endif
