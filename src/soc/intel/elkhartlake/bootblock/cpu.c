/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/fast_spi.h>
#include <soc/bootblock.h>

void bootblock_cpu_init(void)
{
	/*
	 * Elkhartlake platform doesn't support booting from any other media
	 * (like eMMC on APL/GLK platform) than only booting from SPI device
	 * and on IA platform SPI is memory mapped hence enabling temporarily
	 * caching on memory-mapped spi boot media.
	 */
	fast_spi_cache_bios_region();
}
