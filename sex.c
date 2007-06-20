
#include <stdio.h>
#include <string.h>
#include <alsa/asoundef.h>
#include <assert.h>

#include "common.h"
#include "midi.h"
#include "xv.h"
#include "sex.h"

extern char *manufacturer[];

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
	buf[i++] = 0x7e;	/* Non-realtime */
	buf[i++] = dev_id;	/* Device ID */
	buf[i++] = 0x06;	/* Sub ID#1 (General information) */
	buf[i++] = 0x01;	/* Sub ID#2 (Identity request) */
	buf[i++] = MIDI_CMD_COMMON_SYSEX_END;

	midi_sysex_send(i, buf);
	len = midi_sysex_recv(500, buf);

	/* Juno-G 1.06 replies f0 7e 10 06 02 41 15 02 00 00 00 03 00 00 f7 */

	if (buf[0]!=0xf0 || buf[1]!=0x7e || buf[3]!=0x06 || buf[4]!=0x02) {
		printf("Error\n");
		return;
	}
	
	printf("Identification reply\n");
	printf(" Device Id     : %02x\n", buf[2]);

	printf(" Manufacturer  : ");
	if (buf[5] > 0x45 || !manufacturer[buf[5]])
		printf("%02x (unknown)\n", buf[5]);
	else if (buf[5] == 0x7d)
		printf("educational/development\n");
	else
		printf("%s\n", manufacturer[buf[5]]);

	printf(" Family number : %02x %02x\n", buf[6], buf[7]);
	printf(" Model number  : %02x %02x\n", buf[8], buf[9]);
	printf(" Version       : %02x %02x %02x %02x\n",
			buf[10], buf[11], buf[12], buf[13]);
}

void send_patch(struct fsex_libdata *lib, int num, int dev_id)
{
	uint8 *patch, *data;
	int i, size, len;
	uint32 base_addr;
	int offset[] = { PATCH_COMMON, PATCH_COMMON_MFX, PATCH_COMMON_CHORUS,
		PATCH_COMMON_REVERB, PATCH_TMT, PATCH_TONE_1, PATCH_TONE_2,
		PATCH_TONE_3, PATCH_TONE_4, -1 };

	data = lib->data;
	
	for (i = 0; ++i < num; ) {
		size = val32_be(data);
		data += 4 + size;
	}

	size = val32_be(data);
	patch = data + 4;

	base_addr = TEMP_PATCH_RHYTHM_PART1 + TEMP_PATCH;

	printf("Send patch %04d: %-12.12s (%s)\n",
		num, &patch[4 + PATCH_NAME_1],
		patch_category[patch[4 + PATCH_CATEGORY]].short_name);

	for (i = 0; offset[i] >= 0; i++) {
		len = val32_be(patch);
		patch += 4;
		send_sysex(dev_id, base_addr + offset[i], len, patch);
		patch += len;
	}
}

