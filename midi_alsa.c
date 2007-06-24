
#include <stdio.h>
#include <string.h>
#include <alsa/asoundlib.h>

#include "common.h"
#include "midi.h"

static snd_seq_t *seq;
static snd_midi_event_t *mev;
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

static int midi_recv(snd_seq_event_t **e)
{
	snd_seq_event_t *ev;
	int /*n, */err;

	if ((err = snd_seq_event_input(seq, &ev)) < 0) {
		fprintf(stderr, "snd_seq_event_input: %s\n", strerror(err));
		return -1;
	}
	_D(_D_WARN "ev->type = %d", ev->type);
#ifdef _TRACE
	_D(_D_INFO "ev->data.ext.len = %d", ev->data.ext.len);
	{ int i;
	  printf("ev->data.ext.ptr =");
	  for (i = 0; i < ev->data.ext.len; i++)
		printf(" %02x", ((uint8 *)ev->data.ext.ptr)[i]);
	  printf("\n");
	}
#endif

	*e = ev;

        return 0;
}

int midi_open(char *name, char *addr)
{
	snd_seq_addr_t d;
	int err;

	if (snd_seq_open(&seq, "default", SND_SEQ_OPEN_DUPLEX, 0) < 0)
		return -1;

	if (snd_seq_parse_address(seq, &d, addr) < 0)
		return -2;

	if (snd_midi_event_new(512, &mev))
		return -3;

	dest_client = d.client;
	dest_port = d.port;
	
	my_client = snd_seq_client_id(seq);
	snd_seq_set_client_name(seq, name);

	my_port = snd_seq_create_simple_port(seq, name,
		SND_SEQ_PORT_CAP_DUPLEX |
			SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ |
			SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE,
		SND_SEQ_PORT_TYPE_MIDI_GENERIC | SND_SEQ_PORT_TYPE_APPLICATION);

	if (my_port < 0) {
		fprintf(stderr, "can't create port\n");
		snd_seq_close(seq);
		return -1;
	}

	_D(_D_INFO "My address %d:%d", my_client, my_port);
	printf("Send to MIDI address %d:%d\n", dest_client, dest_port);

	err = snd_seq_connect_to(seq, my_port, dest_client, dest_port);
	if (err < 0) {
		fprintf(stderr, "Can't connect to %d:%d (%s)\n",
				dest_client, dest_port, strerror(-err));
		snd_seq_close(seq);
		return -1;
        }

	err = snd_seq_connect_from(seq, my_port, dest_client, dest_port);
	if (err < 0) {
		fprintf(stderr, "Can't connect from %d:%d (%s)\n",
				dest_client, dest_port, strerror(-err));
		snd_seq_close(seq);
		return -1;
        }

	return 0;
}

void midi_close()
{
	snd_midi_event_free(mev);
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

	_D(_D_INFO "ch = %d, msb = %d, lsb = %d", ch, msb, lsb);
	snd_seq_ev_set_controller(&ev, ch, 0, msb);
	midi_send(&ev, 0);
	snd_seq_ev_set_controller(&ev, ch, 32, lsb);
	midi_send(&ev, 0);
}

void midi_pgm(int ch, int patch)
{
	snd_seq_event_t ev;

	_D(_D_INFO "ch = %d, patch = %d", ch, patch);
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

void midi_sysex_send(int len, void *ptr)
{
	snd_seq_event_t ev;

#if _TRACE
	{ int i;
	  printf("*** SysEx send:");
	  for (i = 0; i < len; i++) printf(" %02x", ((uint8 *)ptr)[i]);
	  printf("\n");
	}
#endif
	
	snd_seq_ev_set_sysex(&ev, len, ptr);
	midi_send(&ev, 1);
}

int midi_sysex_recv(int len, void *ptr)
{
	snd_seq_event_t *ev;
	int r;

	r = midi_recv(&ev);

	if (ev->data.ext.len > len)
		return -1;

	len = ev->data.ext.len;

	memcpy(ptr, ev->data.ext.ptr, len);
#if _TRACE
	{ int i;
	  printf("*** SysEx receive:");
	  for (i = 0; i < len; i++) printf(" %02x", ((uint8 *)ptr)[i]);
	  printf("\n");
	}
#endif

	snd_seq_free_event(ev);

	return r;
}
