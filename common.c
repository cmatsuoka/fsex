
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "common.h"

int32 val32_be(uint8 *b)
{
	return (((int)b[0]) << 24) | (((int)b[1]) << 16) |
		(((int)b[2]) << 8) | b[3];
}

int32 val32_lsn(uint8 *b)
{
	return (((int)b[0] & 0x0f) << 12) | (((int)b[1] & 0x0f) << 8) |
		(((int)b[2] & 0x0f) << 4) | (b[3] & 0x0f);
}

int32 val32_le(uint8 *b)
{
	return (((int)b[3]) << 24) | (((int)b[2]) << 16) |
		(((int)b[1]) << 8) | b[0];
}

int32 val24_le(uint8 *b)
{
	return (((int)b[2]) << 16) | (((int)b[1]) << 8) | b[0];
}

int16 val16_le(uint8 *b)
{
	return (((int)b[1]) << 8) | b[0];
}

int write32_le(int fd, uint32 val)
{
	uint8 x;

        x = (val & 0x000000ff) >> 0; write(fd, &x, 1);
        x = (val & 0x0000ff00) >> 8; write(fd, &x, 1);
        x = (val & 0x00ff0000) >> 16; write(fd, &x, 1);
        x = (val & 0xff000000) >> 24; write(fd, &x, 1);

	return 4;
}

int write32_be(int fd, uint32 val)
{
	uint8 x;

        x = (val & 0xff000000) >> 24; write(fd, &x, 1);
        x = (val & 0x00ff0000) >> 16; write(fd, &x, 1);
        x = (val & 0x0000ff00) >> 8; write(fd, &x, 1);
        x = (val & 0x000000ff) >> 0; write(fd, &x, 1);

	return 4;
}

int write16_le(int fd, uint16 val)
{
	uint8 x;

        x = (val & 0x00ff) >> 0; write(fd, &x, 1);
        x = (val & 0xff00) >> 8; write(fd, &x, 1);

	return 2;
}

int suffix(char *filename, char *suffix)
{
	char *s, *c;

	s = strdup(filename);
	if (s == NULL)
		return 0;

	for (c = s; *c; c++)
		*c = tolower(*c);

	return strstr(s, suffix) != NULL;
}

int has_suffix(char *filename)
{
	char *s;
	int l;

	_D(_D_INFO "checking %s", filename);
	l = strlen(filename);
	if (l == 0) return 0;

	for (s = filename + l - 1; s > filename && *s != '/'; s--) {
		if (*s == '.') {
			_D(_D_INFO "suffix found");
			return 1;
		}
	}

	_D(_D_INFO "no suffix");
	return 0;
}

uint8 *mapfile(char *filename)
{
	int fd;
	uint8 *map;
	struct stat st;

	fd = open(filename, O_RDONLY);
	if (fd < 0) return NULL;

	fstat(fd, &st);
        map = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
	close(fd);

	_D(_D_WARN "mapped file %s at %p", filename, map);

	return map;
}

char *base_name(char *dest, char *src, char *ext)
{
	char *s;

	_D(_D_WARN "src = %s", src);
	for (s = src + strlen(src); s >= src && *s != '/'; s--);
	for (s++; *s && *s != '.'; s++) *dest++ = *s;
	if (ext) for (*dest++ = '.'; ext && *ext; ext++) *dest++ = *ext++;
	*dest = 0;

	return dest;
}

