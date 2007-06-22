
#include <ctype.h>
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


int patch_blksz[] = {
	PATCH_COMMON_SIZE,
	PATCH_COMMON_MFX_SIZE,
	PATCH_COMMON_CHORUS_SIZE,
	PATCH_COMMON_REVERB_SIZE,
	PATCH_TMT_SIZE,
	PATCH_TONE_SIZE,
	PATCH_TONE_SIZE,
	PATCH_TONE_SIZE,
	PATCH_TONE_SIZE,
	-1
};

int patch_offset[] = {
	PATCH_COMMON,
	PATCH_COMMON_MFX,
	PATCH_COMMON_CHORUS,
	PATCH_COMMON_REVERB,
	PATCH_TMT,
	PATCH_TONE_1,
	PATCH_TONE_2,
	PATCH_TONE_3,
	PATCH_TONE_4,
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
		blksz = patch_blksz;
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

int create_libfile(struct fsex_libdata *lib, char *filename, int force)
{
	int fd;
	char *str, c = 0;
	struct stat buf;

	if (!force && !stat(filename, &buf)) {
		fprintf(stderr, "%s already exists, use -f to overwrite\n",
				filename);
		return -1;
	}

	fd = open(filename, O_WRONLY | O_CREAT, 0666);
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

void parse_spec(char *s, char **fname, char **list)
{
	_D(_D_INFO "spec = \"%s\"", s);
	*fname = strtok(s, ":");
	*list = strtok(NULL, "");
	if (*list == NULL) *list = "!";
	_D(_D_INFO "  fname = \"%s\", list = \"%s\"", *fname, *list);
}

void set_list_flag(struct fsex_libdata *lib, char *list)
{
	char *token;
	int i, negate = 0;

	if (*list == '!') {
		negate = 1;
		list++;
	}

	for (i = 0; i < lib->num; i++)
		lib->patch[i].skip = !negate;

	token = strtok(list, ",");
	while (token) {
		int a, b;
		char buf[40], cat[4];

		if (isalpha(*token)) {
			a = 1;	/* don't run the skip setting loop below */
			b = 0;

			_D(_D_INFO "token = \"%s\"", token);
			cat[0] = token[0];
			cat[1] = token[1];
			cat[2] = token[2] ? token[2] : ' ';
			cat[3] = 0;

			for (i = 0; i < lib->num; i++) {
				int cnum = lib->patch[i].common[PATCH_CATEGORY];
				if (strncmp(patch_category[cnum].short_name, cat, 3))
					continue;
				lib->patch[i].skip = negate;
			}
		} else if (strchr(token, '-')) {
			b = strcspn (token, "-");
			strncpy(buf, token, b);
			a = strtoul(buf, NULL, 0);
			strncpy(buf, token + b + 1, strlen(token) - b - 1);
			b = strtoul(buf, NULL, 0);
		} else {
			a = b = strtoul(token, NULL, 0);
		}

		for (; b >= a; b--) {
			if (b > 0 && b <= lib->num)
				lib->patch[b - 1].skip = negate;
		}
		token = strtok (NULL, ",");
	}
}

