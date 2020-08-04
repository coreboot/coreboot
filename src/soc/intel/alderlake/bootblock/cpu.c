/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on Intel Alder Lake Processor PCH Datasheet
 * Document number: 621483
 * Chapter number: 7
 */

#include <intelblocks/fast_spi.h>
#include <soc/bootblock.h>

void bootblock_cpu_init(void)
{
	/*
	 * Alderlake platform doesn't support booting from any other media
	 * than SPI flash and on IA platform SPI is memory mapped hence
	 * enabling temporary caching of memory-mapped spi boot media.
	 */
	fast_spi_cache_bios_region();
}
