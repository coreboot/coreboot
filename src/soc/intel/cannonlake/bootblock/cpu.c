/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/fast_spi.h>
#include <soc/bootblock.h>

void bootblock_cpu_init(void)
{
	/* Temporarily cache the memory-mapped boot media. */
	fast_spi_cache_bios_region();
}
