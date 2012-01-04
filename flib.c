
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>

#include "common.h"
#include "xv.h"
#include "library.h"
#include "lib.h"

#define NAME "flib"


static void usage()
{
	printf(
"Usage: " NAME " [options] [action] [sources [destination]]\n"
"\navailable options:\n"
"    -f --force		Overwrite destination if already exists\n"
"    -q --quiet		Don't print messages to stdout\n"
"\navailable actions:\n"
"    -e --extract	Extract patches from library\n"
"    -h --help		Show short description and exit\n"
"    -l --list		List patches in library file\n"
"    -V --version	Print version information\n"
	);
}

#define OPTIONS "efhlqV"
static struct option lopt[] = {
	{ "extract",		1, 0, 'e' },
	{ "force",		1, 0, 'f' },
	{ "help",		0, 0, 'h' },
	{ "list",		0, 0, 'l' },
	{ "version",		0, 0, 'V' },
	{ "quiet",		0, 0, 'q' },
};

int main(int argc, char **argv)
{
	int i, o, optidx;
	int action, force, num_in, num_out;
	char **file_in, *file_out;
	struct fsex_libdata *lib_in;
	int err;

	action = force = 0;

	while ((o = getopt_long(argc, argv, OPTIONS, lopt, &optidx)) > 0) {
		switch (o) {
		case 'e':
		case 'l':
			action = o;
			break;
		case 'f':
			force = 1;
			break;
		case 'h':
			usage();
			exit(0);
		case 'V':
			printf(NAME " " VERSION "\n");
			exit(0);
		case 'q':
			dup2(open("/dev/null", O_WRONLY), STDOUT_FILENO);
			break;
		default:
			exit(1);
		}
	}

	if (!action) {
		fprintf(stderr, NAME ": no action specified\n");
		fprintf(stderr, "Run `" NAME " -h' for more information\n");
		exit(1);
	}

	/* list has no output */
	num_in = argc - 1 - optind + (action == 'l');
	num_out = (action == 'e');
	_D(_D_INFO "num_in = %d, num_out = %d", num_in, num_out);

	lib_in = malloc(num_in * sizeof(struct fsex_libdata));
	file_in = &argv[optind];
	if (num_out) file_out = argv[argc - 1];

	if (num_in < 1) {
		fprintf(stderr, NAME ": input library file name required\n");
		exit(1);
	}

	if (num_out != 1 && action == 'e') {
		fprintf(stderr, NAME ": output library file name required\n");
		exit(1);
	}

	for (i = 0; i < num_in; i++) {
		char *fname, *spec;
		_D(_D_WARN "file_in[%d] = \"%s\"", i, file_in[i]);
		parse_spec(file_in[i], &fname, &spec);
		map_lib_file(fname, &lib_in[i]);
		set_list_flag(&lib_in[i], spec);
	}

	err = 0;

	switch (action) {
	case 'l':
		for (i = 0; i < num_in; i++) {
			if (list_patches(&lib_in[i]) < 0)
				err = 1;
		}
		break;
	case 'e':
		if (extract_patch(lib_in, num_in, file_out, force) < 0)
			err = 1;
		break;
	default:
		fprintf(stderr, NAME ": unknown action\n");
		err = 1;
	}

	for (i = 0; i < num_in; i++)
		free(lib_in[i].patch);
	free(lib_in);

	return err;
}
