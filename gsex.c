
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "common.h"
#include "jglib.h"

void usage()
{
	printf("Usage: gsex [-h | -l | -s nnn] <filename>\n");
	printf("    -h		show short description and exit\n");
	printf("    -l		list patches in librarian file\n");
	printf("    -s nnn	send temporary patch to Juno-G\n");
}

int main(int argc, char **argv)
{
	uint8 *jgl;
	int num;
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

	num = check_jgl(jgl);
	if (num < 0) {
		fprintf(stderr, "data seems to be corrputed\n");
		exit(1);
	}

	if (opt_list) {
		list_patches(jgl, num);
		exit(0);
	}

	if (opt_send) {
		send_patch(jgl, opt_send);
		exit(0);
	}

	return 0;
}
