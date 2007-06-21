
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "common.h"
#include "xv.h"
#include "library.h"


#define CHECK_SIZE(x,y) do {\
	if ((x) != DATA_SIZE((y))) { \
		fprintf(stderr, "data block has size %d (expected %d)\n", \
			(x), DATA_SIZE((y))); \
		return -1; \
	} } while (0)


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



void load_patches(struct fsex_libdata *lib)
{
	uint8 *patch, *data;
	struct fsex_patch *p;
	int i;

	data = lib->data;
	p = lib->patch;

	for (i = 0; i < lib->num; i++) {
		p[i].size = val32_be(data);
		p[i].patch = data;
		data += 4;
		patch = data;
		data += p[i].size;

#define STORE(x) do { \
		p[i].x##_size = val32_be(patch); \
		patch += 4; \
		p[i].x = patch; \
		patch += p[i].x##_size; \
	} while (0)

		STORE(common);
		STORE(mfx);
		STORE(chorus);
		STORE(reverb);
		STORE(tmt);
		STORE(tone1);
		STORE(tone2);
		STORE(tone3);
		STORE(tone4);
	}
}

int map_lib_file(char *filename, struct fsex_libdata *lib)
{
	char *idstr;

	lib->data = mapfile(filename);
	if (lib->data == NULL) {
		perror(filename);
		exit(1);
	}

	lib->model = MODEL_NONE;
	lib->filename = filename;
	printf("* %s: ", filename);

	if (!memcmp(lib->data, "JunoGLibrarianFile0000          ", 32)) {
		idstr = "Juno-G librarian file";
		lib->model = MODEL_JUNOG;
	} else if (!memcmp(lib->data,"FantomSLibrarianFile0000        ", 32)) {
		idstr = "Fantom-S librarian file";
		lib->model = MODEL_FANTOMS;
	} else if (!memcmp(lib->data,"FantomXLibrarianFile0000        ", 32)) {
		idstr = "Fantom-X librarian file";
		lib->model = MODEL_FANTOMX;
	} else {
		printf("unknown\n");
		exit(1);
	}
	lib->data += 32;
	lib->num = val32_be(lib->data);
	lib->data += 128;

	if (check_lib(lib) < 0) {
		fprintf(stderr, "data seems to be corrupted\n");
		exit(1);
	}

	printf("%s (%d patches)\n", idstr, lib->num);

	lib->patch = malloc(lib->num * sizeof(struct fsex_patch));
	if (lib->patch == NULL) {
		perror("error");
		exit(1);
	}

	load_patches(lib);

	return 0;
}

int check_lib(struct fsex_libdata *lib)
{
	uint8 *data, *patch;
	int i, j, size, len;
	int *blksz;

	data = lib->data;

	switch(lib->model) {
	case MODEL_JUNOG:
	case MODEL_FANTOMX:
	case MODEL_FANTOMS:
		blksz = blksz_jg;
		break;
	default:
		return -1;
	}

	for (j = 0; j < lib->num; j++) {
		size = val32_be(data);
		data += 4;
		patch = data;
		data += size;

		/* Patch Common */
		len = val32_be(patch);
		if (len != blksz[0]) break;
		patch += 4 + len;

		for (i = 1; blksz[i] >= 0; i++) {
			len = val32_be(patch);
			CHECK_SIZE(len, blksz[i]);
			patch += 4 + len;
		}
	}

	return j;
}

int create_libfile(struct fsex_libdata *lib, char *filename)
{
	int fd;
	char *str, c = 0;

	fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (fd < 0) {
		perror("error");
		return -1;
	}

	switch(lib->model) {
	case MODEL_JUNOG:
		str = "JunoGLibrarianFile0000          ";
		break;
	case MODEL_FANTOMX:
		str = "FantomXLibrarianFile0000        ";
		break;
	case MODEL_FANTOMS:
		str = "FantomSLibrarianFile0000        ";
		break;
	default:
		fprintf(stderr, "error: unsupported library format\n");
		exit(1);
	}

	write(fd, str, 32);
	lseek(fd, 159, SEEK_SET);
	write(fd, &c, 1);

	return fd;
}

void close_libfile(int fd, int count)
{
	lseek(fd, 32, SEEK_SET);
	write32_be(fd, count);
	close(fd);
}

int write_patch(int fd, struct fsex_patch *p)
{
	int ret;

	ret = (write(fd, p->patch, p->size) == p->size);
	ret |= (write32_be(fd, 0) == 4); /* FIXME: comment field */

	return ret;
}
