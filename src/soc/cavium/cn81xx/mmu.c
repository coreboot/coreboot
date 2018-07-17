/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015         MediaTek Inc.
 * Copyright 2018-present Facebook, Inc.
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

#include <symbols.h>
#include <soc/addressmap.h>
#include <soc/mmu.h>
#include <soc/sdram.h>
#include <arch/mmu.h>

void soc_mmu_init(void)
{
	const unsigned long devmem = MA_DEV | MA_S | MA_RW;
	const unsigned long secure_mem = MA_MEM | MA_S | MA_RW;

	mmu_init();

	/*
	 * Need to use secure mem attribute, as firmware is running in ARM TZ
	 * region.
	 */
	mmu_config_range((void *)_ttb, _ttb_size, secure_mem);
	mmu_config_range((void *)_dram, sdram_size_mb() * MiB, secure_mem);
	/* IO space has the MSB set and is divided into 4 sub-regions:
	 * * NCB
	 * * SLI
	 * * RSL
	 * * AP
	 */
	mmu_config_range((void *)IO_SPACE_START, IO_SPACE_SIZE, devmem);

	mmu_enable();
}
