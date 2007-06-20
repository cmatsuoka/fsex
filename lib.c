
#include <stdio.h>

#include "xv.h"

void list_patches(struct xv_libdata *lib)
{
	uint8 *patch, *data;
	int i, count, size, len;

	data = lib->data;

	printf("\n");
	printf("Pat#  Cat  Patch name   M/P Part  Waveform 1 Waveform 2 "
			"Waveform 3 Waveform 4\n");
	printf("----- ---- ------------- -- ----- ---------- ---------- "
			"---------- ----------\n");

	for (count = 0; count < lib->num; count++) {
		size = val32_be(data);
		data += 4;
		patch = data;
		data += size;

		/* Patch Common */
		len = val32_be(patch);
		patch += 4;

		printf("%04d  %s  %-12.12s  %s  ",
			count + 1,
			patch_category[patch[PATCH_CATEGORY]].short_name,
			&patch[PATCH_NAME_1],
			patch[MONO_POLY] ? "P" : "m");

		patch += len;

		/* Patch Common MFX */
		len = val32_be(patch);
		patch += 4;
		patch += len;

		/* Patch Common Chorus */
		len = val32_be(patch);
		patch += 4;
		patch += len;

		/* Patch Common Reverb */
		len = val32_be(patch);
		patch += 4;
		patch += len;
		
		/* Patch TMT */
		len = val32_be(patch);
		patch += 4;

		printf("%c%c%c%c ",
			patch[TMT1_TONE_SWITCH] ? '1' : '-',
			patch[TMT2_TONE_SWITCH] ? '2' : '-',
			patch[TMT3_TONE_SWITCH] ? '3' : '-',
			patch[TMT4_TONE_SWITCH] ? '4' : '-');

		patch += len;

		/* Patch Tone */
		for (i = 0; i < 4; i++) {
			int n;

			len = val32_be(patch);
			patch += 4;

			switch (patch[WAVE_GROUP_TYPE]) {
			case 0:
				n = val32_lsn(&patch[WAVE_NUMBER_L]);
				switch(lib->model) {
				case MODEL_JUNOG:
					printf(" %-10.10s", junog_wave[n]);
					break;
				case MODEL_FANTOMX:
				case MODEL_FANTOMS:
					printf(" %-10.10s", fantom_wave[n]);
					break;
				default:
					printf(" %-10.10s", "(unknown)");
				}
				break;
			case 1:
				printf(" %-10.10s", "SRX");
				break;
			case 2:
				printf(" %-10.10s", "sample");
				break;
			default:
				printf(" %-10.10s", "invalid");
			}

			patch += len;
		}
		printf("\n");
	}
}

