
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "common.h"
#include "xv.h"


int main(int argc, char **argv)
{
	uint8 *jgl, *patch;
	int size, map_size;
	int count;

	jgl = mapfile(argv[1]);
	if (jgl == 0) {
		perror(argv[1]);
		exit(1);
	}

	if (memcmp(jgl, "JunoGLibrarianFile0000", 22)) {
		fprintf(stderr, "not a Juno-G library\n");
		exit(1);
	}

	jgl += 160;
	count = 0;
	
	while (1) {
		size = val32_be(jgl);
		jgl += 4;
		patch = jgl;
		jgl += size;

		map_size = val32_be(patch);
		patch += 4;

		if (map_size != PATCH_COMMON_SIZE)
			break;

		printf("#%04d:  %-12.12s  (%s)\n",
				++count,
				&patch[PATCH_NAME_1],
				patch_category[patch[PATCH_CATEGORY]]);
	}

	return 0;
}
