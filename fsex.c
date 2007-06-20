
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "common.h"
#include "midi.h"
#include "xv.h"
#include "library.h"
#include "sex.h"

#define NAME "fsex"
#define DEFAULT_ADDR "20:0"


static void usage()
{
	printf(
"Usage: " NAME " [options] [filename]\n"
"available options:\n"
"    -a --address port	ALSA sequencer port (default " DEFAULT_ADDR ")\n"
"    -D --detect		Detect synth model\n"
"    -d --device id	MIDI device ID (default 0x10)\n"
"    -h --help		Show short description and exit\n"
"    -s --send num	Send temporary patch to Juno-G\n"
"    -V --version	Print version information\n"
	);
}

#define OPTIONS "a:Dd:hs:V"
static struct option lopt[] = {
	{ "address",		1, 0, 'a' },
	{ "detect",		0, 0, 'D' },
	{ "device",		1, 0, 'd' },
	{ "help",		0, 0, 'h' },
	{ "send",		1, 0, 's' },
	{ "version",            0, 0, 'V' },
};

int main(int argc, char **argv)
{
	int o, optidx, opt_send, opt_detect;
	char *filename, *addr;
	struct fsex_libdata lib;
	int dev_id;

	addr = DEFAULT_ADDR;
	opt_send = opt_detect = 0;
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
	fprintf(stderr, "Run `" NAME " -h' for more information\n");

	return 0;
}
