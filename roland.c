
#include "xv.h"

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
