
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "common.h"
#include "xv.h"
#include "library.h"
#include "lib.h"

#define NAME "flib"


void usage()
{
	printf(
"Usage: " NAME " [options] [filename]\n"
"available options:\n"
"    -h --help		Show short description and exit\n"
"    -l	--list		List patches in librarian file\n"
"    -V --version	Print version information\n"
	);
}

#define OPTIONS "hlV"
static struct option lopt[] = {
	{ "help",		0, 0, 'h' },
	{ "list",		0, 0, 'l' },
	{ "version",            0, 0, 'V' },
};

int main(int argc, char **argv)
{
	int o, optidx, opt_list;
	char *filename;
	struct fsex_libdata lib;

	opt_list = 0;
	filename = NULL;

	while ((o = getopt_long(argc, argv, OPTIONS, lopt, &optidx)) > 0) {
		switch (o) {
		case 'h':
			usage();
			exit(0);
		case 'l':
			opt_list = 1;
			break;
		case 'V':
			printf(NAME " " VERSION "\n");
			exit(0);
		default:
			exit(1);
		}
	}

	if (optind < argc)
		filename = argv[optind];

	if (opt_list) {
		if (!filename) {
			fprintf(stderr, "error: library file required\n");
			exit(1);
		}
		map_lib_file(filename, &lib);
		list_patches(&lib);
		exit(0);
	}

	fprintf(stderr, NAME ": no action specified\n");
	fprintf(stderr, "Run `" NAME " -h' for more information\n");

	return 0;
}
