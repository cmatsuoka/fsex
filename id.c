
#include <stdlib.h>

#include "id.h"

static struct fsex_id synth_id[] = {
	{
		"Juno-G",
		"JunoGLibrarianFile0000          ",
		MODEL_JUNOG,
		3, { 0x00, 0x00, 0x15 }
	},
	{
		"Fantom-X",
		"FantomXLibrarianFile0000        ",
		MODEL_FANTOMX,
		2, { 0x00, 0x6b }
	},
	{
		"Fantom-S",
		"FantomSLibrarianFile0000        ",
		MODEL_FANTOMS,
		2, { 0x00, 0x6b }
	},
	{
		NULL
	}
};



struct fsex_id *find_id_by_model(int model)
{
	int i;

	for (i = 0; synth_id[i].name; i++) {
		if (synth_id[i].model == model) {
			return &synth_id[i];
		}
	}

	return NULL;
}

struct fsex_id *find_id_by_libid(void *lib_id)
{
	int i;

	for (i = 0; synth_id[i].name; i++) {
		if (!memcmp(synth_id[i].lib_id, lib_id, 32)) {
			return &synth_id[i];
		}
	}

	return NULL;
}

