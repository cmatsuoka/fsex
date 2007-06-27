
#ifndef __FSEX_LIBRARY_H
#define __FSEX_LIBRARY_H

#include "common.h"

struct fsex_category {
	char *short_name;
	char *long_name;
};

struct fsex_libdata {
	int model;
	int num;
	char *filename;
	struct fsex_patch *patch;
	uint8 *data;
};

extern struct fsex_category patch_category[];

int	map_lib_file	(char *, struct fsex_libdata *);
int	check_lib       (struct fsex_libdata *);
int	create_libfile	(struct fsex_libdata *, char *, int);
void	close_libfile	(int, int);

void	parse_spec	(char *, char **, char **);
void	set_list_flag	(struct fsex_libdata *, char *);

void	load_patches	(struct fsex_libdata *);

#endif
