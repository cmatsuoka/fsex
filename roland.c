
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

void send_sysex(uint32 addr, int len, uint8 *data)
{
	static uint8 buf[550];
	int sum, start;
	int i;

	assert(len < 500);

	i = 0;
	buf[i++] = MIDI_CMD_COMMON_SYSEX;
	buf[i++] = 0x41;	/* Roland ID */
	buf[i++] = 0x10;	/* Device ID (FIXME: user-assignable) */
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

	midi_sysex(i, buf);

}

void recv_sysex(uint32 addr, int len, uint8 *data)
{
	static uint8 buf[550];
	int sum;
	int i;

	assert(len < 500);

	i = 0;
	buf[i++] = MIDI_CMD_COMMON_SYSEX;
	buf[i++] = 0x41;	/* Roland ID */
	buf[i++] = 0x10;	/* Device ID (FIXME: user-assignable) */
	buf[i++] = 0x00;	/* Juno-G ID (FIXME: allow other devices) */
	buf[i++] = 0x00;	/* Juno-G ID */
	buf[i++] = 0x15;	/* Juno-G ID */
	buf[i++] = 0x11;	/* Command ID (RQ1) */
	
	memcpy(buf + i, data, len);
	i += len;

	sum = checksum(i - 2, data + 2);

	buf[i++] = sum;
	buf[i++] = MIDI_CMD_COMMON_SYSEX_END;

	midi_sysex(i, buf);
}

