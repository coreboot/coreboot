/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <intelblocks/fast_spi.h>
#include <soc/bootblock.h>

void bootblock_cpu_init(void)
{
	/*
	 * Jasperlake platform doesn't support booting from any other media
	 * (like eMMC on APL/GLK platform) than only booting from SPI device
	 * and on IA platform SPI is memory mapped hence enabling temporarily
	 * cacheing on memory-mapped spi boot media.
	 *
	 * This assumption will not hold good for APL/GLK platform where boot
	 * from eMMC is also possible options.
	 */
	fast_spi_cache_bios_region();
}
