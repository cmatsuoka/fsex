
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "common.h"
#include "xv.h"
#include "id.h"
#include "library.h"
#include "patch.h"


int patch_blksz[] = {
	PATCH_COMMON_SIZE,
	PATCH_COMMON_MFX_SIZE,
	PATCH_COMMON_CHORUS_SIZE,
	PATCH_COMMON_REVERB_SIZE,
	PATCH_TMT_SIZE,
	PATCH_TONE_SIZE,
	PATCH_TONE_SIZE,
	PATCH_TONE_SIZE,
	PATCH_TONE_SIZE,
	-1
};

int patch_offset[] = {
	PATCH_COMMON,
	PATCH_COMMON_MFX,
	PATCH_COMMON_CHORUS,
	PATCH_COMMON_REVERB,
	PATCH_TMT,
	PATCH_TONE_1,
	PATCH_TONE_2,
	PATCH_TONE_3,
	PATCH_TONE_4,
	-1
};

void load_patches(struct fsex_libdata *lib)
{
	uint8 *patch, *data;
	struct fsex_patch *p;
	int i;

	data = lib->data;
	p = lib->patch;

	for (i = 0; i < lib->num; i++) {
		p[i].size = val32_be(data);
		p[i].patch = data;
		data += 4;
		patch = data;
		data += p[i].size;

#define STORE(x) do { \
		p[i].x##_size = val32_be(patch); \
		patch += 4; \
		p[i].x = patch; \
		patch += p[i].x##_size; \
	} while (0)

		STORE(common);
		STORE(mfx);
		STORE(chorus);
		STORE(reverb);
		STORE(tmt);
		STORE(tone1);
		STORE(tone2);
		STORE(tone3);
		STORE(tone4);
	}
}

int write_patch(int fd, struct fsex_patch *p)
{
	int ret;

	_D(_D_WARN "fd = %d", fd);
	ret = (write(fd, p->patch, p->size) == p->size);
	ret |= (write32_be(fd, 0) == 4); /* FIXME: comment field */

	return ret;
}

