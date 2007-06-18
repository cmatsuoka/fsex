
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

int check_jgl(uint8 *jgl)
{
	uint8 *patch;
	int i, count, size, len;

	for (count = 0; ; count++) {
		size = val32_be(jgl);
		jgl += 4;
		patch = jgl;
		jgl += size;

		/* Patch Common */
		len = val32_be(patch);
		if (len != PATCH_COMMON_SIZE_JG) break;
		patch += 4 + len;

		/* Patch Common MFX */
		len = val32_be(patch);
		CHECK_SIZE(len, PATCH_COMMON_MFX_SIZE_JG);
		patch += 4 + len;

		/* Patch Common Chorus */
		len = val32_be(patch);
		CHECK_SIZE(len, PATCH_COMMON_CHORUS_SIZE_JG);
		patch += 4 + len;

		/* Patch Common Reverb */
		len = val32_be(patch);
		CHECK_SIZE(len, PATCH_COMMON_REVERB_SIZE_JG);
		patch += 4 + len;
		
		/* Patch TMT */
		len = val32_be(patch);
		CHECK_SIZE(len, PATCH_TMT_SIZE_JG);
		patch += 4 + len;

		/* Patch Tone */
		for (i = 0; i < 4; i++) {
			len = val32_be(patch);
			CHECK_SIZE(len, PATCH_TONE_SIZE_JG);
			patch += 4 + len;
		}
	}

	return count;
}

void list_patches(uint8 *jgl, int num)
{
	uint8 *patch;
	int i, count, size, len;

	printf("Pat#  Cat  Patch name   M/P Part  Waveform 1 Waveform 2 "
			"Waveform 3 Waveform 4\n");
	printf("----- ---- ------------- -- ----- ---------- ---------- "
			"---------- ----------\n");

	for (count = 0; count < num; count++) {
		size = val32_be(jgl);
		jgl += 4;
		patch = jgl;
		jgl += size;

		/* Patch Common */
		len = val32_be(patch);
		patch += 4;

		printf(" %04d  %s  %-12.12s  %s  ",
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

		printf("%c%c%c%c",
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
				printf("  %-9.9s", junog_wave[n]);
				break;
			case 1:
				printf("  %-9.9s", "SRX");
				break;
			case 2:
				printf("  %-9.9s", "sample");
				break;
			default:
				printf("  %-9.9s", "invalid");
			}

			patch += len;
		}
		printf("\n");
	}
}

void send_patch(uint8 *jgl, int num)
{
	uint8 *patch;
	int i, count = 0;
	int size, len;

	while (++count < num) {
		size = val32_be(jgl);
		jgl += 4 + size;
	}

	size = val32_be(jgl);
	patch = jgl + 4;

	/* Patch Common */
	len = val32_be(patch);
	patch += 4;
	printf("Patch %04d: %-12.12s\n", num, &patch[PATCH_NAME_1]);
	printf("Sending patch common data...\n");

	send_sysex(len, patch);

	patch += len;

	/* Patch Common MFX */
	len = val32_be(patch);
	patch += 4;
	printf("Sending patch MFX data...\n");

	patch += len;

	/* Patch Common Chorus */
	len = val32_be(patch);
	patch += 4;
	printf("Sending patch chorus data...\n");

	patch += len;

	/* Patch Common Reverb */
	len = val32_be(patch);
	patch += 4;
	printf("Sending patch reverb data...\n");

	patch += len;
	
	/* Patch TMT */
	len = val32_be(patch);
	patch += 4;
	printf("Sending patch TMT data...\n");

	patch += len;

	/* Patch Tone */
	for (i = 0; i < 4; i++) {
		len = val32_be(patch);
		patch += 4;
		printf("Sending patch tone %d data...\n", i + 1);

		patch += len;
	}
}

