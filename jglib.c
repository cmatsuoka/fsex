
#include <stdio.h>

#include "common.h"
#include "xv.h"
#include "jglib.h"

#define DATA_SIZE(x) (((x)>0x7f)?(0x80+((x)&0xff)):(x))

#define CHECK_SIZE(x,y) do {\
	if ((x) != DATA_SIZE((y))) { \
		printf("data block has size %d (expected %d)\n", \
			(x), DATA_SIZE((y))); \
		return -1; \
	} } while (0)

int check_jgl(uint8 *jgl)
{
	uint8 *patch;
	int i, count, size, map_size;

	printf("Checking jgl file...\n");

	for (count = 0; ; count++) {
		size = val32_be(jgl);
		jgl += 4;
		patch = jgl;
		jgl += size;

		/* Patch Common */
		map_size = val32_be(patch);
		if (map_size != PATCH_COMMON_SIZE) break;
		patch += 4 + map_size;

		/* Patch Common MFX */
		map_size = val32_be(patch);
		CHECK_SIZE(map_size, PATCH_COMMON_MFX_SIZE);
		patch += 4 + map_size;

		/* Patch Common Chorus */
		map_size = val32_be(patch);
		CHECK_SIZE(map_size, PATCH_COMMON_CHORUS_SIZE);
		patch += 4 + map_size;

		/* Patch Common Reverb */
		map_size = val32_be(patch);
		CHECK_SIZE(map_size, PATCH_COMMON_REVERB_SIZE);
		patch += 4 + map_size;
		
		/* Patch TMT */
		map_size = val32_be(patch);
		CHECK_SIZE(map_size, PATCH_TMT_SIZE);
		patch += 4 + map_size;

		/* Patch Tone */
		for (i = 0; i < 4; i++) {
			map_size = val32_be(patch);
			CHECK_SIZE(map_size, PATCH_TONE_SIZE);
			patch += 4 + map_size;
		}
	}

	return count;
}

void list_patches(uint8 *jgl, int num)
{
	uint8 *patch;
	int i, size, map_size;

	for (i = 0; i < num; i++) {
		size = val32_be(jgl);
		jgl += 4;
		patch = jgl;
		jgl += size;

		/* Patch Common */
		map_size = val32_be(patch);
		patch += 4;

		printf(" %04d  %-3.3s  %-12.12s  %s  ",
			i + 1,
			patch_category[patch[PATCH_CATEGORY]].short_name,
			&patch[PATCH_NAME_1],
			patch[MONO_POLY] ? "----" : "mono");

		patch += map_size;

		/* Patch Common MFX */
		map_size = val32_be(patch);
		patch += 4;
		patch += map_size;

		/* Patch Common Chorus */
		map_size = val32_be(patch);
		patch += 4;
		patch += map_size;

		/* Patch Common Reverb */
		map_size = val32_be(patch);
		patch += 4;
		patch += map_size;
		
		/* Patch TMT */
		map_size = val32_be(patch);
		patch += 4;

		printf("%c%c%c%c\n",
			patch[TMT1_TONE_SWITCH] ? '1' : '-',
			patch[TMT2_TONE_SWITCH] ? '2' : '-',
			patch[TMT3_TONE_SWITCH] ? '3' : '-',
			patch[TMT4_TONE_SWITCH] ? '4' : '-');

		patch += map_size;
	}
}

void send_patch(uint8 *jgl, int num)
{
	uint8 *patch;
	int i, count = 0;
	int size, map_size;

	while (++count < num) {
		size = val32_be(jgl);
		jgl += 4 + size;
	}

	size = val32_be(jgl);
	patch = jgl + 4;

	/* Patch Common */
	map_size = val32_be(patch);
	patch += 4;
	printf("Patch %04d: %-12.12s\n", num, &patch[PATCH_NAME_1]);
	printf("Sending patch common data...\n");

	patch += map_size;

	/* Patch Common MFX */
	map_size = val32_be(patch);
	patch += 4;
	printf("Sending patch MFX data...\n");

	patch += map_size;

	/* Patch Common Chorus */
	map_size = val32_be(patch);
	patch += 4;
	printf("Sending patch chorus data...\n");

	patch += map_size;

	/* Patch Common Reverb */
	map_size = val32_be(patch);
	patch += 4;
	printf("Sending patch reverb data...\n");

	patch += map_size;
	
	/* Patch TMT */
	map_size = val32_be(patch);
	patch += 4;
	printf("Sending patch TMT data...\n");

	patch += map_size;

	/* Patch Tone */
	for (i = 0; i < 4; i++) {
		map_size = val32_be(patch);
		patch += 4;
		printf("Sending patch tone %d data...\n", i + 1);

		patch += map_size;
	}
}

