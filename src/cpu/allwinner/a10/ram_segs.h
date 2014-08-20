/*
 * How we use DRAM on Allwinner CPUs
 *
 * Copyright (C) 2014  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#include <config.h>
#include <symbols.h>

/*
 * Put CBMEM at top of RAM
 */
static inline void *a1x_get_cbmem_top(void)
{
	return _dram + (CONFIG_DRAM_SIZE_MB << 20);
}

/*
 * By CBFS cache, we mean a cached copy, in RAM, of the entire CBFS region.
 */
static inline void *a1x_get_cbfs_cache_top(void)
{
	/* Arbitrary 16 MiB gap for cbmem tables and bouncebuffer */
	return a1x_get_cbmem_top() - (16 << 20);
}

static inline void *a1x_get_cbfs_cache_base(void)
{
	return a1x_get_cbfs_cache_top() - CONFIG_ROM_SIZE;
}
