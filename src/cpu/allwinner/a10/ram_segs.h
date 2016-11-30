/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 *
 * How we use DRAM on Allwinner CPUs
 */

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
