
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "common.h"
#include "midi.h"
#include "xv.h"

#define NAME "gsex"
#define VERSION "0.0"

int map_lib_file(char *filename, struct xv_libdata *lib)
{
	lib->data = mapfile(filename);
	if (lib->data == 0) {
		perror(filename);
		exit(1);
	}

	lib->model = MODEL_NONE;
	printf("Checking library file... ");

	if (!memcmp(lib->data, "JunoGLibrarianFile0000", 22)) {
		printf("Juno-G librarian file\n");
		lib->model = MODEL_JUNOG;
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

	return 0;
}

void usage()
{
	printf("Usage: " NAME " [options] [filename]\n");
	printf("    -a ccc:ppp	alsa MIDI client:port\n"); 
	printf("    -D		detect synth model\n");
	printf("    -h		show short description and exit\n");
	printf("    -l		list patches in librarian file\n");
	printf("    -s nnn	send temporary patch to Juno-G\n");
}

#define OPTIONS "a:Dlhs:V"
static struct option lopt[] = {
	{ "address",		1, 0, 'a' },
	{ "detect",		0, 0, 'D' },
	{ "help",		0, 0, 'h' },
	{ "version",            0, 0, 'V' },
	{ "list",		0, 0, 'l' },
	{ "send",		1, 0, 's' },
};

int main(int argc, char **argv)
{
	int o, optidx, opt_list, opt_send, opt_detect;
	char *filename, *addr;
	struct xv_libdata lib;

	addr = NULL;
	opt_list = opt_send = opt_detect = 0;
	filename = NULL;

	while ((o = getopt_long(argc, argv, OPTIONS, lopt, &optidx)) > 0) {
		switch (o) {
		case 'a':
			addr = optarg;
			break;
		case 'D':
			opt_detect = 1;
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

	if (optind >= argc) {
		usage();
		exit(1);
	}

	filename = argv[optind];

	if (opt_detect) {
		if (midi_open(NAME, addr) < 0) {
			fprintf(stderr, "error: can't open sequencer\n");
			return 1;
		}
		sysex_get_id();
		midi_close();
		exit(0);
	}

	if (opt_list) {
		map_lib_file(filename, &lib);
		list_patches(&lib);
		exit(0);
	}

	if (opt_send) {
		map_lib_file(filename, &lib);
		if (midi_open(NAME, addr) < 0) {
			fprintf(stderr, "error: can't open sequencer\n");
			return 1;
		}
		send_patch(&lib, opt_send);
		midi_close();
		exit(0);
	}

	return 0;
}
