
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "common.h"
#include "midi.h"
#include "xv.h"

#define NAME "gsex"
#define DEFAULT_ADDR "20:0"

#define STR(x) # x

int map_lib_file(char *filename, struct xv_libdata *lib)
{
	lib->data = mapfile(filename);
	if (lib->data == NULL) {
		perror(filename);
		exit(1);
	}

	lib->model = MODEL_NONE;
	printf("File format   : ");

	if (!memcmp(lib->data, "JunoGLibrarianFile0000", 22)) {
		printf("Juno-G librarian file\n");
		lib->model = MODEL_JUNOG;
		lib->data += 160;
	} else if (!memcmp(lib->data,"FantomSLibrarianFile0000", 24)) {
		printf("Fantom-S librarian file\n");
		lib->model = MODEL_FANTOMS;
		lib->data += 160;
	} else if (!memcmp(lib->data,"FantomXLibrarianFile0000", 24)) {
		printf("Fantom-X librarian file\n");
		lib->model = MODEL_FANTOMX;
		lib->data += 160;
	} else {
		printf("unknown\n");
		exit(1);
	}

	lib->num = check_lib(lib);
	if (lib->num < 0) {
		printf("data seems to be corrupted\n");
		exit(1);
	}

	printf("Num of patches: %d\n", lib->num);

	return 0;
}

void usage()
{
	printf(
"Usage: " NAME " [options] [filename]\n"
"available options:\n"
"    -a --address id	ALSA sequencer MIDI device address (default "
			DEFAULT_ADDR ")\n"
"    -D --detect	Detect synth model\n"
"    -d --device id	MIDI device ID (default 0x10)\n"
"    -h	--help		Show short description and exit\n"
"    -l	--list		List patches in librarian file\n"
"    -s --send num	Send temporary patch to Juno-G\n"
"    -V --version	Print version information\n"
	);
}

#define OPTIONS "a:Dd:lhs:V"
static struct option lopt[] = {
	{ "address",		1, 0, 'a' },
	{ "detect",		0, 0, 'D' },
	{ "device",		1, 0, 'd' },
	{ "help",		0, 0, 'h' },
	{ "list",		0, 0, 'l' },
	{ "send",		1, 0, 's' },
	{ "version",            0, 0, 'V' },
};

int main(int argc, char **argv)
{
	int o, optidx, opt_list, opt_send, opt_detect;
	char *filename, *addr;
	struct xv_libdata lib;
	int dev_id;

	addr = DEFAULT_ADDR;
	opt_list = opt_send = opt_detect = 0;
	dev_id = 0x10;
	filename = NULL;

	while ((o = getopt_long(argc, argv, OPTIONS, lopt, &optidx)) > 0) {
		switch (o) {
		case 'a':
			addr = optarg;
			break;
		case 'D':
			opt_detect = 1;
			break;
		case 'd':
			dev_id = strtoul(optarg, NULL, 0);
			break;
		case 'h':
			usage();
			exit(0);
		case 'l':
			opt_list = 1;
			break;
		case 's':
			opt_send = strtoul(optarg, NULL, 0);
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

	if (opt_detect) {
		if (midi_open(NAME, addr) < 0) {
			fprintf(stderr, "error: can't open sequencer\n");
			exit(1);
		}
		sysex_get_id(dev_id);
		midi_close();
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

	if (opt_send) {
		if (!filename) {
			fprintf(stderr, "error: library file required\n");
			exit(1);
		}
		map_lib_file(filename, &lib);
		if (midi_open(NAME, addr) < 0) {
			fprintf(stderr, "error: can't open sequencer\n");
			return 1;
		}
		send_patch(&lib, opt_send, dev_id);
		midi_close();
		exit(0);
	}

	fprintf(stderr, NAME ": no action specified\n");
	fprintf(stderr, "Run `" NAME "-h' for more information\n");

	return 0;
}
