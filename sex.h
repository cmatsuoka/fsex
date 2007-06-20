
#ifndef __FSEX_SEX_H
#define __FSEX_SEX_H

void	send_sysex	(int, uint32, int, uint8 *);
int	recv_sysex	(int, uint8 *);
void    send_patch      (struct xv_libdata *, int, int);
int	checksum	(int, uint8 *);
void	sysex_get_id	(int);

#endif
