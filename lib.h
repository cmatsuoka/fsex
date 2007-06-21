
#ifndef __FSEX_LIB_H
#define __FSEX_LIB_H

#include "library.h"

void	list_patches    (struct fsex_libdata *);
int	delete_patch	(struct fsex_libdata *, struct fsex_libdata *, char *);
int	extract_patch	(struct fsex_libdata *, struct fsex_libdata *, char *);

#endif
