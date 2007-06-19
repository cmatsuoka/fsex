
#include <stdio.h>
#include <string.h>
#include <alsa/asoundef.h>
#include <assert.h>

#include "common.h"
#include "midi.h"
#include "xv.h"

int checksum(int len, uint8 *data)
{
	int i, acc;

	acc = 0;
	for (i = 0; i < len; i++) {
		acc += *data++;
		if (acc > 0x7f)
			acc -= 0x80;
	}

	acc %= 0x80;

	return 0x80 - acc;
}

void send_sysex(int dev_id, uint32 addr, int len, uint8 *data)
{
	static uint8 buf[550];
	int sum, start;
	int i;

	assert(len < 500);

	i = 0;
	buf[i++] = MIDI_CMD_COMMON_SYSEX;
	buf[i++] = 0x41;	/* Roland ID */
	buf[i++] = dev_id;	/* Device ID */
	buf[i++] = 0x00;	/* Juno-G ID (FIXME: allow other devices) */
	buf[i++] = 0x00;	/* Juno-G ID */
	buf[i++] = 0x15;	/* Juno-G ID */
	buf[i++] = 0x12;	/* Command ID (DT1) */

	start = i;

	buf[i++] = (addr & 0xff000000) >> 24;
	buf[i++] = (addr & 0x00ff0000) >> 16;
	buf[i++] = (addr & 0x0000ff00) >> 8;
	buf[i++] = (addr & 0x000000ff);
	
	memcpy(buf + i, data, len);
	i += len;

	sum = checksum(len + 4, buf + start);

	buf[i++] = sum;
	buf[i++] = MIDI_CMD_COMMON_SYSEX_END;

	midi_sysex_send(i, buf);
}

int recv_sysex(int len, uint8 *data)
{
	return midi_sysex_recv(len, data);
}

void sysex_get_id(int dev_id)
{
	static uint8 buf[550];
	int i, len;
	
	i = 0;
	buf[i++] = MIDI_CMD_COMMON_SYSEX;
	buf[i++] = 0x7e;	/* ID number */
	buf[i++] = dev_id;	/* Device ID */
	buf[i++] = 0x06;	/* Sub ID#1 (General information) */
	buf[i++] = 0x01;	/* Sub ID#2 (Identity request) */
	buf[i++] = MIDI_CMD_COMMON_SYSEX_END;

	midi_sysex_send(i, buf);
	len = midi_sysex_recv(500, buf);

	for (i = 0; i < len; i++) {
		printf(" %02x", buf[i]);
	}
	printf("\n");
}
