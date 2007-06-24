#ifndef __FSEX_ID_H
#define __FSEX_ID_H

#include "common.h"

enum fsex_model {
	MODEL_NONE,
	MODEL_XV5080,
	MODEL_FANTOMS,
	MODEL_FANTOMX,
	MODEL_JUNOG
};

struct fsex_id {
	char *name;
	char *lib_id;
	int model;
	int id_size;
	uint8 id[5];
};

struct fsex_id *find_id_by_model(int);
struct fsex_id *find_id_by_libid(void *);

#endif
