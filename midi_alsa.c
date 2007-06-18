
#include <stdio.h>
#include <alsa/asoundlib.h>

#include "midi.h"

static snd_seq_t *seq;
static int my_client, my_port;
static int dest_client, dest_port;


static void midi_send(snd_seq_event_t *ev, int d)
{
	snd_seq_ev_set_direct(ev);
	snd_seq_ev_set_source(ev, my_port);
	snd_seq_ev_set_dest(ev, dest_client, dest_port);
        snd_seq_event_output(seq, ev);
	if (!d) return;

	snd_seq_drain_output(seq);
}

int midi_open(int client, int port)
{
	char *name = "test";
	int caps;

	dest_client = client;
	dest_port = port;

	if (snd_seq_open(&seq, "hw", SND_SEQ_OPEN_OUTPUT, 0) < 0)
		return -1;

	my_client = snd_seq_client_id(seq);
	snd_seq_set_client_name(seq, name);

	caps = SND_SEQ_PORT_CAP_READ;
	if (my_client == SND_SEQ_ADDRESS_SUBSCRIBERS)
		caps |= SND_SEQ_PORT_CAP_SUBS_READ;

	my_port = snd_seq_create_simple_port(seq, name, caps,
		SND_SEQ_PORT_TYPE_MIDI_GENERIC | SND_SEQ_PORT_TYPE_APPLICATION);

	if (my_port < 0) {
		fprintf(stderr, "error: can't create port\n");
		snd_seq_close(seq);
		return -1;
	}

	printf("My address %d:%d\n", my_client, my_port);
	printf("Destination address %d:%d\n", dest_client, dest_port);

	if (dest_client != SND_SEQ_ADDRESS_SUBSCRIBERS) {
                if (snd_seq_connect_to(seq, my_port, dest_client, dest_port) < 0) {
			fprintf(stderr, "error: can't subscribe to MIDI port"
					" (%d:%d)\n", dest_client, dest_port);
                        snd_seq_close(seq);
                        return -1;
                }
        }

	return 0;
}

void midi_close()
{
	snd_seq_close(seq);
}

void midi_noteon(int ch, int note, int vel)
{
	snd_seq_event_t ev;

	snd_seq_ev_set_noteon(&ev, ch, note, vel);
	midi_send(&ev, 1);
}

void midi_noteoff(char ch, int note, int vel)
{
	snd_seq_event_t ev;

	snd_seq_ev_set_noteoff(&ev, ch, note, vel);
	midi_send(&ev, 1);
}

void midi_bank(int ch, int msb, int lsb)
{
	snd_seq_event_t ev;

	snd_seq_ev_set_controller(&ev, ch, 0, msb);
	midi_send(&ev, 0);
	snd_seq_ev_set_controller(&ev, ch, 32, lsb);
	midi_send(&ev, 0);
}

void midi_pgm(int ch, int patch)
{
	snd_seq_event_t ev;

	snd_seq_ev_set_pgmchange(&ev, ch, patch);
	midi_send(&ev, 1);
}

void midi_ctl(int ch, int ctl, int val)
{
	snd_seq_event_t ev;

	snd_seq_ev_set_controller(&ev, ch, ctl, val);
	midi_send(&ev, 1);
}

void midi_bend(int ch, int val)
{
	snd_seq_event_t ev;

	snd_seq_ev_set_pitchbend(&ev, ch, val);
	midi_send(&ev, 1);
}

void midi_sysex(int len, void *ptr)
{
	snd_seq_event_t ev;

	snd_seq_ev_set_sysex(&ev, len, ptr);
	midi_send(&ev, 1);
}
