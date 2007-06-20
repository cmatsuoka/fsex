
#ifndef __FSEX_LIBRARY_H
#define __FSEX_LIBRARY_H

#include "xv.h"

struct fsex_patch {
	int common_size;
	int mfx_size;
	int chorus_size;
	int reverb_size;
	int tmt_size;
	int tone1_size;
	int tone2_size;
	int tone3_size;
	int tone4_size;
	uint8 common[0x60];
	uint8 mfx[0x120];
	uint8 chorus[0x60];
	uint8 reverb[0x60];
	uint8 tmt[0x40];
	uint8 tone1[0x120];
	uint8 tone2[0x120];
	uint8 tone3[0x120];
	uint8 tone4[0x120];
};

struct fsex_category {
	char *short_name;
	char *long_name;
};

struct fsex_libdata {
	int model;
	int num;
	uint8 *data;
};

extern struct fsex_category patch_category[];

int	map_lib_file	(char *, struct fsex_libdata *);
int     check_lib       (struct fsex_libdata *);

#endif
