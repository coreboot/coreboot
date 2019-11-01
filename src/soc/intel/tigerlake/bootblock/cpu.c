/*
 * This file is part of the coreboot project.
 *
  * Copyright (C) 2019 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * This file is created based on Intel Tiger Lake Processor PCH Datasheet
 * Document number: 575857
 * Chapter number: 6
 */

#include <intelblocks/fast_spi.h>
#include <soc/bootblock.h>

void bootblock_cpu_init(void)
{
	/*
	 * Tigerlake platform doesn't support booting from any other media
	 * (like eMMC on APL/GLK platform) than only booting from SPI device
	 * and on IA platform SPI is memory mapped hence enabling temporarily
	 * cacheing on memory-mapped spi boot media.
	 *
	 * This assumption will not hold good for APL/GLK platform where boot
	 * from eMMC is also possible options.
	 */
	fast_spi_cache_bios_region();
}
