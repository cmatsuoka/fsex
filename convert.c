
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "xv.h"
#include "library.h"
#include "id.h"
#include "patch.h"
#include "convert.h"


struct equiv_waves {
	char *a;
	char *b;
};


struct equiv_waves equiv[] = {
	{ "Dyno Rhd mpA", "Dyno EP mp A" },
	{ "Dyno Rhd mpB", "Dyno EP mp B" },
	{ "Dyno Rhd mpC", "Dyno EP mp C" },
	{ "Dyno Rhd mfA", "Dyno EP mf A" },
	{ "Dyno Rhd mfB", "Dyno EP mf B" },
	{ "Dyno Rhd mfC", "Dyno EP mf C" },
	{ "Dyno Rhd ffA", "Dyno EP ff A" },
	{ "Dyno Rhd ffB", "Dyno EP ff B" },
	{ "Dyno Rhd ffC", "Dyno EP ff C" },
};


static char *waveform_name(int model, int group, int n)
{
	switch (group) {
	case 0:
		switch(model) {
		case MODEL_JUNOG:
			return junog_wave[n];
		case MODEL_FANTOMX:
		case MODEL_FANTOMS:
			return fantom_wave[n];
		default:
			return "unknown";
		}
	case 1:
		return "SRX";
	case 2:
		return "sample";
	default:
		return "invalid";
	}
}


static int match(char *a, char *b)
{
	int i;

	if (!strcmp(a, b))
		return 1;

	for (i = 0; equiv[i].a; i++) {
		if (!strcmp(a, equiv[i].a) && !strcmp(b, equiv[i].b))
			return 1;
	}
		
	return 0;
}

static int convert_waveform(int from, int to, int n)
{
	int i;
	char *name;
	char **wave;

	switch (from) {
	case MODEL_JUNOG:
		name = junog_wave[n];
		break;
	case MODEL_FANTOMX:
	case MODEL_FANTOMS:
		name = fantom_wave[n];
		break;
	default:
		return -1;
	}

	switch (to) {
	case MODEL_JUNOG:
		wave = junog_wave;
		break;
	case MODEL_FANTOMX:
	case MODEL_FANTOMS:
		wave = fantom_wave;
		break;
	default:
		return -1;
	}

	for (i = 0; wave[i]; i++) {
		if (match(name, wave[i]))
			return i;
	}

	return -1;
}


static void do_conversion(int tone, int from, int to, uint8 *patch)
{
	int n, g;

	n = val32_lsn(&patch[WAVE_NUMBER_L]);
	g = patch[WAVE_GROUP_TYPE];

	if (n > 0 && g == 0) {
		int x;

		printf("\t\t\t%d: %-12.12s (%4d)  ==>  ",
			tone, waveform_name(from, g, n), n);

		x = convert_waveform(from, to, n);
		if (x < 0) {
			printf("** FAIL **\n");
		} else {
			printf("%-12.12s (%4d)\n",
				waveform_name(to, g, x), x);
			setval32_lsn(&patch[WAVE_NUMBER_L], x);
		}
	}
}

int convert_patches(struct fsex_libdata *lib, int dest, char *output, int force)
{
	uint8 *patch, *data;
	int i, fd;
	uint8 newpatch[2000];
	struct fsex_libdata newlib;

	data = lib->data;

	newlib.model = dest;
	fd = create_libfile(&newlib, output, force);
	if (fd < 0) {
		return fd;
	}

	printf("\nPatches from %s:\n\n", lib->filename);

	for (i = 0; i < lib->num_patch; i++) {
		if (lib->patch[i].skip)
			continue;

		memcpy(newpatch, lib->patch[i].patch, lib->patch[i].size);

		/* Patch Common */
		patch = newpatch + (lib->patch[i].common - lib->patch[i].patch);
		printf(" %04d  [%-12.12s]\r", i + 1, &patch[PATCH_NAME_1]);

		/* Patch Tone 1 */
		patch = newpatch + (lib->patch[i].tone1 - lib->patch[i].patch);
		do_conversion(1, lib->model, dest, patch);

		/* Patch Tone 2 */
		patch = newpatch + (lib->patch[i].tone2 - lib->patch[i].patch);
		do_conversion(2, lib->model, dest, patch);

		/* Patch Tone 3 */
		patch = newpatch + (lib->patch[i].tone3 - lib->patch[i].patch);
		do_conversion(3, lib->model, dest, patch);

		/* Patch Tone 4 */
		patch = newpatch + (lib->patch[i].tone4 - lib->patch[i].patch);
		do_conversion(4, lib->model, dest, patch);

		/* write raw data */
		write(fd, newpatch, lib->patch[i].size);
		write32_be(fd, 0);	/* FIXME: comment field */
	}

	printf("\nCreate new library: %s\n", output);

	close_libfile(fd, i);

	return 0;
}
