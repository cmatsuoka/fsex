
#include <stdio.h>
#include <string.h>

#include "xv.h"
#include "library.h"
#include "lib.h"



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


void list_patches(struct fsex_libdata *lib)
{
	uint8 *patch, *data;
	int i, n, g;

	data = lib->data;

	printf("\n");
	printf("Pat#  Cat  Patch name   M/P Part  Waveform 1 Waveform 2 "
			"Waveform 3 Waveform 4\n");
	printf("----- ---- ------------- -- ----- ---------- ---------- "
			"---------- ----------\n");

	for (i = 0; i < lib->num; i++) {
		if (lib->patch[i].skip)
			continue;

		/* Patch Common */
		patch = lib->patch[i].common;
		printf("%04d  %s  %-12.12s  %s  ",
			i + 1,
			patch_category[patch[PATCH_CATEGORY]].short_name,
			&patch[PATCH_NAME_1],
			patch[MONO_POLY] ? "P" : "m");

		/* Patch TMT */
		patch = lib->patch[i].tmt;
		printf("%c%c%c%c ",
			patch[TMT1_TONE_SWITCH] ? '1' : '-',
			patch[TMT2_TONE_SWITCH] ? '2' : '-',
			patch[TMT3_TONE_SWITCH] ? '3' : '-',
			patch[TMT4_TONE_SWITCH] ? '4' : '-');

		/* Patch Tone 1 */
		patch = lib->patch[i].tone1;
		n = val32_lsn(&patch[WAVE_NUMBER_L]);
		g = patch[WAVE_GROUP_TYPE];
		printf(" %-10.10s", waveform_name(lib->model, g, n));

		/* Patch Tone 2 */
		patch = lib->patch[i].tone2;
		n = val32_lsn(&patch[WAVE_NUMBER_L]);
		g = patch[WAVE_GROUP_TYPE];
		printf(" %-10.10s", waveform_name(lib->model, g, n));

		/* Patch Tone 3 */
		patch = lib->patch[i].tone3;
		n = val32_lsn(&patch[WAVE_NUMBER_L]);
		g = patch[WAVE_GROUP_TYPE];
		printf(" %-10.10s", waveform_name(lib->model, g, n));

		/* Patch Tone 4 */
		patch = lib->patch[i].tone4;
		n = val32_lsn(&patch[WAVE_NUMBER_L]);
		g = patch[WAVE_GROUP_TYPE];
		printf(" %-10.10s\n", waveform_name(lib->model, g, n));
	}
}

#if 0
int delete_patch(struct fsex_libdata *lib, struct fsex_libdata *mlib, char *output)
{
	int i, fd;

	fd = create_libfile(lib, output);

	printf("Delete patch from %s:", lib->filename);
	for (i = 0; i < lib->num; i++) {
		if (~lib->patch[i].flags & FSEX_FLAG_DELETE) {
			write_patch(fd, &lib->patch[i]);
		} else {
			printf(" %d", i + 1);
		}
	}
	printf("\n");

	if (mlib) {
		printf("Merge with %s (%d patches)\n",
					mlib->filename, mlib->num);
		for (i = 0; i < mlib->num; i++)
			write_patch(fd, &mlib->patch[i]);
	}

	close_libfile(fd);

	return 0;
}

int extract_patch(struct fsex_libdata *lib, struct fsex_libdata *mlib, char *output)
{
	int i, fd;

	fd = create_libfile(lib, output);

	printf("Extract patch from %s:", lib->filename);
	for (i = 0; i < lib->num; i++) {
		if (lib->patch[i].flags & FSEX_FLAG_EXTRACT) {
			write_patch(fd, &lib->patch[i]);
			printf(" %d", i + 1);
		}
	}
	printf("\n");

	if (mlib) {
		printf("Merge with %s (%d patches)\n",
					mlib->filename, mlib->num);
		for (i = 0; i < mlib->num; i++)
			write_patch(fd, &mlib->patch[i]);
	}

	close_libfile(fd);

	return 0;
}
#endif
