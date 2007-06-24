
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alsa/asoundef.h>
#include <assert.h>

#include "common.h"
#include "midi.h"
#include "xv.h"
#include "id.h"
#include "sex.h"

extern char *manufacturer[];

static struct bank_sel bank[] = {
	{ "USER",   1, 128, 0x57, 0x00,    0 },
	{ "USER", 129, 256, 0x57, 0x01, -128 },
	{ "CARD",   1, 128, 0x57, 0x20,    0 },
	{ "CARD", 129, 256, 0x57, 0x21, -128 },
	{ "PR-A",   1, 128, 0x57, 0x40,    0 },
	{ "PR-B",   1, 128, 0x57, 0x41,    0 },
	{ "PR-C",   1, 128, 0x57, 0x42,    0 },
	{ "PR-D",   1, 128, 0x57, 0x43,    0 },
	{ "PR-E",   1, 128, 0x57, 0x44,    0 },
	{ "PR-F",   1, 128, 0x57, 0x45,    0 },
	{ NULL,     0,   0,    0,    0,    0 }
};

static int checksum(int len, uint8 *data)
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

static void send_sysex(int model, int dev_id, uint32 addr, int len, uint8 *data)
{
	static uint8 buf[550];
	int sum, start;
	int i;

	_D(_D_INFO "dev_id: 0x%02x, addr: %08x, len: %d", dev_id, addr, len);
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

static void recv_sysex(int dev_id, uint32 addr, int len, int dsize, uint8 *data)
{
	static uint8 buf[550];
	int sum, start;
	int i;

	_D(_D_INFO "dev_id: 0x%02x, addr: 0x%08x, len: %d", dev_id, addr, len);
	assert(len <= 500);

	i = 0;
	buf[i++] = MIDI_CMD_COMMON_SYSEX;
	buf[i++] = 0x41;	/* Roland ID */
	buf[i++] = dev_id;	/* Device ID */
	buf[i++] = 0x00;	/* Juno-G ID (FIXME: allow other devices) */
	buf[i++] = 0x00;	/* Juno-G ID */
	buf[i++] = 0x15;	/* Juno-G ID */
	buf[i++] = 0x11;	/* Command ID (RQ1) */

	start = i;

	buf[i++] = (addr & 0xff000000) >> 24;
	buf[i++] = (addr & 0x00ff0000) >> 16;
	buf[i++] = (addr & 0x0000ff00) >> 8;
	buf[i++] = (addr & 0x000000ff);
	
	buf[i++] = (len & 0xff000000) >> 24;
	buf[i++] = (len & 0x00ff0000) >> 16;
	buf[i++] = (len & 0x0000ff00) >> 8;
	buf[i++] = (len & 0x000000ff);
	
	sum = checksum(i - start, buf + start);

	buf[i++] = sum;
	buf[i++] = MIDI_CMD_COMMON_SYSEX_END;

	midi_sysex_send(i, buf);
	midi_sysex_recv(dsize, data);
}

void sysex_get_id(int dev_id)
{
	static uint8 buf[20];
	int i, len;
	
	i = 0;
	buf[i++] = MIDI_CMD_COMMON_SYSEX;
	buf[i++] = 0x7e;	/* Non-realtime */
	buf[i++] = dev_id;	/* Device ID */
	buf[i++] = 0x06;	/* Sub ID#1 (General information) */
	buf[i++] = 0x01;	/* Sub ID#2 (Identity request) */
	buf[i++] = MIDI_CMD_COMMON_SYSEX_END;

	midi_sysex_send(i, buf);
	len = midi_sysex_recv(20, buf);

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

void send_patch(struct fsex_libdata *lib, int dev_id)
{
	uint8 *patch, *data;
	int i, size, len;
	uint32 base_addr;

	/* Locate first valid patch, and send only one to temp area */
	data = NULL;
	for (i = 0; i < lib->num; i++) {
		if (lib->patch[i].skip == 0) {
			data = lib->patch[i].patch;
			break;
		}
	}
	if (data == NULL)
		return;

	size = val32_be(data);
	patch = data + 4;

	base_addr = TEMP_PATCH_RHYTHM_PART1 + TEMP_PATCH;

	printf("Send patch %04d: %-12.12s (%s)\n",
		i + 1, &patch[4 + PATCH_NAME_1],
		patch_category[patch[4 + PATCH_CATEGORY]].short_name);

	for (i = 0; patch_offset[i] >= 0; i++) {
		len = val32_be(patch);
		patch += 4;
		send_sysex(lib->model, dev_id, base_addr + patch_offset[i],
								len, patch);
		patch += len;
	}
}

static int select_bank(char *bname, uint8 *msb, uint8 *lsb)
{
	int i;

	for (i = 0; bank[i].name; i++) {
		if (!strcmp(bname, bank[i].name)) {
			_D(_D_WARN "Select bank: %s", bname);
			*msb = bank[i].msb;
			*lsb = bank[i].lsb;
			return 0;
		}
	}

	return -1;
}

int map_synth_patches(char *bankname, struct fsex_libdata *lib)
{
	int i;

	lib->model = MODEL_NONE;
	lib->filename = bankname;
	printf("* %s: ", bankname);

	for (i = 0; bank[i].name; i++) {
		if (!strcmp(bankname, bank[i].name)) {
			_D(_D_WARN "Select bank: %s/%02x%02x",
				bank[i].name, bank[i].msb, bank[i].lsb);
			printf("%d patches\n", bank[i].max - bank[i].min + 1);
			break;
		}
	}
	if (bank[i].name == NULL) {
		fprintf(stderr, "invalid bank %s\n", bankname);
		exit(1);
	}

	lib->num = bank[i].max - bank[i].min + 1;

	lib->patch = malloc(lib->num * sizeof(struct fsex_patch));
	if (lib->patch == NULL) {
		perror("error");
		exit(1);
	}

	return 0;
}

int recv_patch(struct fsex_libdata *lib, int num, int dev_id, uint8 *data)
{
	int i, len, real_len, size;
	uint32 base_addr;
	uint8 msb, lsb;
	uint8 *d = data;
	uint8 buf[550];

	_D(_D_WARN "lib->filename = %s, num = %d, dev_id = %d",
						lib->filename, num, dev_id);

	if (select_bank(lib->filename, &msb, &lsb) < 0) {
		fprintf(stderr, "error: invalid bank %s\n", lib->filename);
		return 0;
	}

	if (num == 0)
		return -1;

	midi_bank(0, msb, lsb);
	midi_pgm(0, num - 1);

	base_addr = TEMP_PATCH_RHYTHM_PART1 + TEMP_PATCH;

	size = 0;

	data += 4;
	for (i = 0; patch_offset[i] >= 0; i++) {
		len = patch_blksz[i];
		assert(len < 500);
		real_len = DATA_SIZE(len);
		_D(_D_INFO "len = %d, real_len = %d", len, real_len);
		size += 4 + real_len;
		*data++ = (real_len & 0xff000000) >> 24;
		*data++ = (real_len & 0x00ff0000) >> 16;
		*data++ = (real_len & 0x0000ff00) >> 8;
		*data++ = (real_len & 0x000000ff);

		_D(_D_INFO "patch_offset[%d] = %08x", i, patch_offset[i]);
		recv_sysex(dev_id, base_addr + patch_offset[i], len, 500, buf);
		memcpy(data, buf + 11, real_len);

#if _TRACE > 1
		_D(_D_INFO "");
		{ int j;
		  printf("*** received data:");
		  for (j = 0; j < real_len; j++) printf(" %02x", data[j]);
		  printf("\n");
		}
#endif
		data += real_len;
	}

	size += 4;	/* comment area */

	d[0] = (size & 0xff000000) >> 24;
	d[1] = (size & 0x00ff0000) >> 16;
	d[2] = (size & 0x0000ff00) >> 8;
	d[3] = (size & 0x000000ff);

	return size;
}


void recv_patches(int dev_id, char **file_in, struct fsex_libdata *lib, int num, char *output)
{
	int i, j, fd;
	struct fsex_libdata lib_out;
	struct fsex_patch p;
	uint8 pdata[2048];
	int num_patches;

	_D(_D_WARN "dev_id: %d, file_in[0]: %s, num: %d, output: %s",
					dev_id, file_in[0], num, output);
	lib_out.model = MODEL_JUNOG;
	p.patch = pdata;
	fd = create_libfile(&lib_out, output, 1);
	if (fd < 0) {
		fprintf(stderr, "error: can't create output file\n");
		exit(1);
	}

	printf("\nReceive patches from synth:\n\n");
	num_patches = 0;
	for (j = 0; j < num; j++) {
		for (i = 0; i < lib[j].num; i++) {
			if (lib[j].patch[i].skip)
				continue;

			p.size = recv_patch(&lib[j], i + 1, dev_id, pdata);
			p.common = pdata + 8;
			printf("%s:%04d  %s  %-12.12s\n",
				lib[j].filename, i + 1,
				patch_category[p.common[PATCH_CATEGORY]].short_name,
				&p.common[PATCH_NAME_1]);
			write_patch(fd, &p);
			num_patches++;
		}
	}

	printf("\nCreate new library: %s\n", output);

	close_libfile(fd, num_patches);
}

