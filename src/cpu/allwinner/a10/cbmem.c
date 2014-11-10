/*
 * Provides cbmem utilities for romstage and ramstage
 *
 * Copyright (C) 2013 Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#include "ram_segs.h"
#include <cbmem.h>

void *cbmem_top(void)
{
	return a1x_get_cbmem_top();
}
