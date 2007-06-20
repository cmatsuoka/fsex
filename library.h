
#ifndef __FSEX_LIBRARY_H
#define __FSEX_LIBRARY_H

#include "common.h"

struct fsex_patch {
	unsigned long flags;
#define FSEX_FLAG_EXTRACT	(1L << 0)
#define FSEX_FLAG_DELETE	(1L << 1);
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

struct fsex_category {
	char *short_name;
	char *long_name;
};

struct fsex_libdata {
	int model;
	int num;
	struct fsex_patch *patch;
	uint8 *data;
};

extern struct fsex_category patch_category[];

int	map_lib_file	(char *, struct fsex_libdata *);
int	check_lib       (struct fsex_libdata *);
int	create_libfile	(struct fsex_libdata *, char *);
void	close_libfile	(int);
int	write_patch	(int, struct fsex_patch *);


#endif
