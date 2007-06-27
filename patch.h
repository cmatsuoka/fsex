
#ifndef __FSEX_PATCH_H
#define __FSEX_PATCH_H

#include "common.h"

struct fsex_patch {
	int skip;
	int size;
	int common_size;
	int mfx_size;
	int chorus_size;
	int reverb_size;
	int tmt_size;
	int tone1_size;
	int tone2_size;
	int tone3_size;
	int tone4_size;
	uint8 *patch;
	uint8 *common;
	uint8 *mfx;
	uint8 *chorus;
	uint8 *reverb;
	uint8 *tmt;
	uint8 *tone1;
	uint8 *tone2;
	uint8 *tone3;
	uint8 *tone4;
};

extern int patch_blksz[];
extern int patch_offset[];

int	write_patch	(int, struct fsex_patch *);

#endif
