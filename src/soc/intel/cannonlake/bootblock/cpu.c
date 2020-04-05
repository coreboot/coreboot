/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <intelblocks/cpulib.h>
#include <intelblocks/fast_spi.h>
#include <soc/bootblock.h>

void bootblock_cpu_init(void)
{
	/* Temporarily cache the memory-mapped boot media. */
	if (CONFIG(BOOT_DEVICE_MEMORY_MAPPED) &&
		CONFIG(BOOT_DEVICE_SPI_FLASH))
		fast_spi_cache_bios_region();
}
