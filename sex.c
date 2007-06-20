
#include <stdio.h>

#include "xv.h"


void send_patch(struct xv_libdata *lib, int num, int dev_id)
{
	uint8 *patch, *data;
	int i, size, len;
	uint32 base_addr;
	int offset[] = { PATCH_COMMON, PATCH_COMMON_MFX, PATCH_COMMON_CHORUS,
		PATCH_COMMON_REVERB, PATCH_TMT, PATCH_TONE_1, PATCH_TONE_2,
		PATCH_TONE_3, PATCH_TONE_4, -1 };

	data = lib->data;
	
	for (i = 0; ++i < num; ) {
		size = val32_be(data);
		data += 4 + size;
	}

	size = val32_be(data);
	patch = data + 4;

	base_addr = TEMP_PATCH_RHYTHM_PART1 + TEMP_PATCH;

	printf("Send patch %04d: %-12.12s (%s)\n",
		num, &patch[4 + PATCH_NAME_1],
		patch_category[patch[4 + PATCH_CATEGORY]].short_name);

	for (i = 0; offset[i] >= 0; i++) {
		len = val32_be(patch);
		patch += 4;
		send_sysex(dev_id, base_addr + offset[i], len, patch);
		patch += len;
	}
}

