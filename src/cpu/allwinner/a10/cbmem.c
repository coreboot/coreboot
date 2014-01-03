/*
 * Provides cbmem utilities for romstage and ramstage
 *
 * Copyright (C) 2013 Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#include "ram_segs.h"
#include <cbmem.h>

#if IS_ENABLED(CONFIG_DYNAMIC_CBMEM)

void *cbmem_top(void)
{
	return a1x_get_cbmem_top();
}

#else

void get_cbmem_table(u64 *base, u64 *size)
{
	*size = CONFIG_COREBOOT_TABLES_SIZE;
	*base = (unsigned)a1x_get_cbmem_top() - CONFIG_COREBOOT_TABLES_SIZE;
}

#endif
