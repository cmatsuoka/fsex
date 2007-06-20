
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "common.h"
#include "xv.h"
#include "library.h"
#include "lib.h"

#define NAME "flib"

void set_list_flags(struct fsex_libdata *lib, char *list, uint32 flag)
{
	char *token;

	token = strtok(list, ",");
	while (token) {
		int a, b;
		char buf[40];

		if (strchr(token, '-')) {
			b = strcspn (token, "-");
			strncpy(buf, token, b);
			a = strtoul(buf, NULL, 0);
			strncpy(buf, token + b + 1, strlen(token) - b - 1);
			b = strtoul(buf, NULL, 0);
		} else {
			a = b = strtoul(token, NULL, 0);
		}

		for (; b >= a; b--) {
			if (b <= lib->num)
				lib->patch[b].flags |= flag;
		}
		token = strtok (NULL, ",");
	}
}

void usage()
{
	printf(
"Usage: " NAME " [options] [library]\n"
"available options:\n"
"    -d --delete list	Delete patches from library\n"
"    -e --extract list	Extract patches from library\n"
"    -h --help		Show short description and exit\n"
"    -l	--list		List patches in library file\n"
"    -o --output name	Set output filename\n"
"    -V --version	Print version information\n"
	);
}

#define OPTIONS "d:e:hlVo:"
static struct option lopt[] = {
	{ "delete",		1, 0, 'd' },
	{ "extract",		1, 0, 'e' },
	{ "help",		0, 0, 'h' },
	{ "list",		0, 0, 'l' },
	{ "output",		1, 0, 'o' },
	{ "version",            0, 0, 'V' },
};

int main(int argc, char **argv)
{
	int o, optidx, opt_list;
	char *filename;
	char *opt_extract, *opt_delete;
	struct fsex_libdata lib;

	opt_list = 0;
	opt_extract = opt_delete = NULL;
	filename = NULL;

	while ((o = getopt_long(argc, argv, OPTIONS, lopt, &optidx)) > 0) {
		switch (o) {
		case 'd':
			opt_delete = optarg;
			break;
		case 'e':
			opt_extract = optarg;
			break;
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

	if (opt_extract) {
		if (!filename) {
			fprintf(stderr, "error: library file required\n");
			exit(1);
		}
		map_lib_file(filename, &lib);
		set_list_flags(&lib, opt_extract, FSEX_FLAG_EXTRACT);
		exit(0);
	}

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
