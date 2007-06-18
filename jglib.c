
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "common.h"
#include "xv.h"


#define DATA_SIZE(x) (((x)>0x7f)?(0x80+((x)&0xff)):(x))
#define CHECK_SIZE(x,y) if ((x) != DATA_SIZE(y)) { \
	printf("Data size mismatch (%d, should be %d)\n", (x), (y)); break; }


void list_patches(uint8 *jgl)
{
	uint8 *patch;
	int count = 0;
	int size, map_size;

	while (1) {
		size = val32_be(jgl);
		jgl += 4;
		patch = jgl;
		jgl += size;

		/* Patch Common */
		map_size = val32_be(patch);
		patch += 4;

		if (map_size != DATA_SIZE(PATCH_COMMON_SIZE))
			break;

		printf(" %04d  %-3.3s  %-12.12s  %s  ",
			++count,
			patch_category[patch[PATCH_CATEGORY]].short_name,
			&patch[PATCH_NAME_1],
			patch[MONO_POLY] ? "----" : "mono");

		patch += map_size;

		/* Patch Common MFX */
		map_size = val32_be(patch);
		patch += 4;
		CHECK_SIZE(map_size, PATCH_COMMON_MFX_SIZE);
		patch += map_size;

		/* Patch Common Chorus */
		map_size = val32_be(patch);
		patch += 4;
		CHECK_SIZE(map_size, PATCH_COMMON_CHORUS_SIZE);
		patch += map_size;

		/* Patch Common Reverb */
		map_size = val32_be(patch);
		patch += 4;
		CHECK_SIZE(map_size, PATCH_COMMON_REVERB_SIZE);
		patch += map_size;
		
		/* Patch TMT */
		map_size = val32_be(patch);
		patch += 4;
		CHECK_SIZE(map_size, PATCH_TMT_SIZE);
		patch += map_size;

		printf("%c%c%c%c\n",
			patch[TMT1_TONE_SWITCH] ? '1' : '-',
			patch[TMT2_TONE_SWITCH] ? '2' : '-',
			patch[TMT3_TONE_SWITCH] ? '3' : '-',
			patch[TMT4_TONE_SWITCH] ? '4' : '-');
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

void usage()
{
	printf("Usage: jglib [-h | -l | -s nnn] <filename>\n");
	printf("    -h		show short description and exit\n");
	printf("    -l		list patches in librarian file\n");
	printf("    -s nnn	send temporary patch to Juno-G\n");
}

int main(int argc, char **argv)
{
	uint8 *jgl;
	int o, opt_list, opt_send;
	char *filename;

	opt_list = opt_send = 0;
	filename = NULL;

	while ((o = getopt(argc, argv, "lhs:")) > 0) {
		switch (o) {
		case 'h':
			usage();
			exit(0);
		case 'l':
			opt_list = 1;
			break;
		case 's':
			opt_send = strtoul(optarg, NULL, 0);
			break;
		default:
			exit(1);
		}
	}

	if (optind >= argc) {
		usage();
		exit(1);
	}

	filename = argv[optind];

	jgl = mapfile(filename);
	if (jgl == 0) {
		perror(filename);
		exit(1);
	}

	if (memcmp(jgl, "JunoGLibrarianFile0000", 22)) {
		fprintf(stderr, "not a Juno-G library\n");
		exit(1);
	}
	jgl += 160;

	if (opt_list) {
		list_patches(jgl);
		exit(0);
	}

	if (opt_send) {
		send_patch(jgl, opt_send);
		exit(0);
	}

	return 0;
}
