
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "common.h"
#include "jglib.h"
#include "midi.h"
#include "xv.h"

#define NAME "gsex"
#define VERSION "0.0"

static int blksz_jg[] = {
	PATCH_COMMON_SIZE_JG,
	PATCH_COMMON_MFX_SIZE_JG,
	PATCH_COMMON_CHORUS_SIZE_JG,
	PATCH_COMMON_REVERB_SIZE_JG,
	PATCH_TMT_SIZE_JG,
	PATCH_TONE_SIZE_JG,
	PATCH_TONE_SIZE_JG,
	PATCH_TONE_SIZE_JG,
	PATCH_TONE_SIZE_JG,
	-1
};
	
uint8 *map_jgl_file(char *filename, int *num)
{
	uint8 *jgl;

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

	printf("Checking jgl file... ");
	*num = check_lib(jgl, blksz_jg);
	if (*num < 0) {
		fprintf(stderr, "data seems to be corrputed\n");
		exit(1);
	}
	printf("ok\n");

	return jgl;
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
	uint8 *jgl;
	int num;
	int o, optidx, opt_list, opt_send, opt_detect;
	char *filename, *addr;

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
		jgl = map_jgl_file(filename, &num);
		list_patches(jgl, num);
		exit(0);
	}

	if (opt_send) {
		jgl = map_jgl_file(filename, &num);
		if (midi_open(NAME, addr) < 0) {
			fprintf(stderr, "error: can't open sequencer\n");
			return 1;
		}
		send_patch(jgl, opt_send);
		midi_close();
		exit(0);
	}

	return 0;
}
