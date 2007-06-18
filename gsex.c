
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "common.h"
#include "jglib.h"
#include "midi.h"


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
	*num = check_jgl(jgl);
	if (*num < 0) {
		fprintf(stderr, "data seems to be corrputed\n");
		exit(1);
	}
	printf("ok\n");

	return jgl;
}

void usage()
{
	printf("Usage: gsex [-a ccc:ppp] [-h | -l | -s nnn] <filename>\n");
	printf("    -a ccc:ppp	Alsa MIDI client:port\n"); 
	printf("    -h		show short description and exit\n");
	printf("    -l		list patches in librarian file\n");
	printf("    -s nnn	send temporary patch to Juno-G\n");
}

int main(int argc, char **argv)
{
	uint8 *jgl;
	int num;
	int o, opt_list, opt_send;
	char *filename, *p;
	int client, port;

	client = port = 0;
	opt_list = opt_send = 0;
	filename = NULL;

	while ((o = getopt(argc, argv, "a:lhs:")) > 0) {
		switch (o) {
		case 'h':
			usage();
			exit(0);
		case 'a':
			client = strtoul(optarg, &p, 0);
			port = strtoul(++p, NULL, 0);
			break;
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

	if (opt_list) {
		jgl = map_jgl_file(filename, &num);
		list_patches(jgl, num);
		exit(0);
	}

	if (opt_send) {
		jgl = map_jgl_file(filename, &num);

		if (midi_open(client, port) < 0) {
			fprintf(stderr, "error: can't open sequencer\n");
			return 1;
		}

		send_patch(jgl, opt_send);

		midi_close();

		exit(0);
	}

	return 0;
}
