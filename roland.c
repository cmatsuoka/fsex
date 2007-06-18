
#include <string.h>
#include <assert.h>

#include "common.h"
#include "midi.h"

int checksum(int len, uint8 *data)
{
	int i, acc;

	acc = 0;
	for (i = 0; i < len; i++) {
		acc += *data++;
		if (acc > 0x7)
			acc -= 0x80;
	}

	return 0x80 - acc;
}

void send_sysex(int len, uint8 *data)
{
	static uint8 buf[550];
	int sum;
	int i;

	assert(len < 500);

	sum = checksum(len, data);

	i = 0;
	buf[i++] = 0x41;		/* Roland sysex ID */
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = 0x15;
	buf[i++] = 0x12;		/* SysEx send */
	
	memcpy(buf + i, data, len);
	i += len;

	buf[i++] = sum;

	midi_sysex(i, buf);
}
