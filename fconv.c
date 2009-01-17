
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
#include "id.h"
#include "library.h"
#include "convert.h"

#define NAME "fconv"


static void usage()
{
	printf(
"Usage: " NAME " [options] [action] [source [destination]]\n"
"\navailable options:\n"
"    -f --force		Overwrite destination if already exists\n"
"    -q --quiet		Don't print messages to stdout\n"
"\navailable actions:\n"
"    -h --help		Show short description and exit\n"
"    -j --jgl		Convert to Juno-G library\n"
"    -V --version	Print version information\n"
	);
}

#define OPTIONS "efhlqV"
static struct option lopt[] = {
	{ "force",		1, 0, 'f' },
	{ "help",		0, 0, 'h' },
	{ "jgl",		0, 0, 'j' },
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
	int dest;

	action = force = dest = 0;

	while ((o = getopt_long(argc, argv, OPTIONS, lopt, &optidx)) > 0) {
		switch (o) {
		case 'j':
			action = o;
			dest = MODEL_JUNOG;
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

	num_in = 1;
	num_out = 1;
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
	case 'j':
		if (convert_patches(lib_in, dest, file_out, force) < 0)
			err = 1;
		break;
	default:
		fprintf(stderr, NAME ": unknown action\n");
		err = 1;
	}

	return err;
}
