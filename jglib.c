
#include <stdio.h>

#include "common.h"
#include "xv.h"
#include "jglib.h"


#define CHECK_SIZE(x,y) do {\
	if ((x) != DATA_SIZE((y))) { \
		fprintf(stderr, "data block has size %d (expected %d)\n", \
			(x), DATA_SIZE((y))); \
		return -1; \
	} } while (0)

int check_lib(uint8 *lib, int *blksz)
{
	uint8 *patch;
	int i, count, size, len;

	for (count = 0; ; count++) {
		size = val32_be(lib);
		lib += 4;
		patch = lib;
		lib += size;

		/* Patch Common */
		len = val32_be(patch);
		if (len != blksz[0]) break;
		patch += 4 + len;

		for (i = 1; blksz[i] >= 0; i++) {
			len = val32_be(patch);
			CHECK_SIZE(len, blksz[i]);
			patch += 4 + len;
		}
	}

	return count;
}

void list_patches(uint8 *lib, int num)
{
	uint8 *patch;
	int i, count, size, len;

	printf("Pat#  Cat  Patch name   M/P Part  Waveform 1 Waveform 2 "
			"Waveform 3 Waveform 4\n");
	printf("----- ---- ------------- -- ----- ---------- ---------- "
			"---------- ----------\n");

	for (count = 0; count < num; count++) {
		size = val32_be(lib);
		lib += 4;
		patch = lib;
		lib += size;

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
				printf(" %-10.10s", junog_wave[n]);
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

void send_patch(uint8 *lib, int num)
{
	uint8 *patch;
	int i, size, len;
	uint32 base_addr;
	int offset[] = { PATCH_COMMON, PATCH_COMMON_MFX, PATCH_COMMON_CHORUS,
		PATCH_COMMON_REVERB, PATCH_TMT, PATCH_TONE_1, PATCH_TONE_2,
		PATCH_TONE_3, PATCH_TONE_4, -1 };

	for (i = 0; ++i < num; ) {
		size = val32_be(lib);
		lib += 4 + size;
	}

	size = val32_be(lib);
	patch = lib + 4;

	base_addr = TEMP_PATCH_RHYTHM_PART1 + TEMP_PATCH;

	printf("Send patch %04d: %-12.12s (%s)\n",
		num, &patch[4 + PATCH_NAME_1],
		patch_category[patch[4 + PATCH_CATEGORY]].short_name);

	for (i = 0; offset[i] >= 0; i++) {
		len = val32_be(patch);
		patch += 4;
		send_sysex(base_addr + offset[i], len, patch);
		patch += len;
	}
}

