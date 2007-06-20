
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
	int i, size;

	data = lib->data;
	p = lib->patch;

	for (i = 0; i < lib->num; i++) {
		size = val32_be(data);
		data += 4;
		patch = data;
		data += size;

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
	lib->data = mapfile(filename);
	if (lib->data == NULL) {
		perror(filename);
		exit(1);
	}

	lib->model = MODEL_NONE;
	printf("File format: ");

	if (!memcmp(lib->data, "JunoGLibrarianFile0000", 22)) {
		printf("Juno-G librarian file\n");
		lib->model = MODEL_JUNOG;
		lib->data += 160;
	} else if (!memcmp(lib->data,"FantomSLibrarianFile0000", 24)) {
		printf("Fantom-S librarian file\n");
		lib->model = MODEL_FANTOMS;
		lib->data += 160;
	} else if (!memcmp(lib->data,"FantomXLibrarianFile0000", 24)) {
		printf("Fantom-X librarian file\n");
		lib->model = MODEL_FANTOMX;
		lib->data += 160;
	} else {
		printf("unknown\n");
		exit(1);
	}

	lib->num = check_lib(lib);
	if (lib->num < 0) {
		fprintf(stderr, "data seems to be corrupted\n");
		exit(1);
	}

	printf("Num patches: %d\n", lib->num);

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
	int i, count, size, len;
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

	for (count = 0; ; count++) {
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

	return count;
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

void close_libfile(int fd)
{
	close(fd);
}

int write_patch(int fd, struct fsex_patch *p)
{
	return 0;
}
