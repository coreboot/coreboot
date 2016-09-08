/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Marvell, Inc.
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

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <memrange.h>
#include <arch/mmu.h>
#include <soc/addressmap.h>
#include <soc/mmu_operations.h>

static void mvmap2315_mmu_config(void)
{
	const unsigned long ram_mem = MA_MEM | MA_NS | MA_RW;
	const unsigned long dev_mem = MA_DEV | MA_S | MA_RW;
	const unsigned long lcm_mem = MA_MEM | MA_NS | MA_RW | MA_MEM_NC;
	const unsigned long flash_mem = MA_MEM | MA_S | MA_RW;

	mmu_config_range((void *)MVMAP2315_RAM_BASE,
			 MVMAP2315_RAM_SIZE, ram_mem);

	mmu_config_range((void *)MVMAP2315_DEVICE_BASE,
			 MVMAP2315_DEVICE_SIZE, dev_mem);

	mmu_config_range((void *)MVMAP2315_LCM_BASE,
			 MVMAP2315_LCM_SIZE, lcm_mem);

	mmu_config_range((void *)MVMAP2315_FLASH_BASE,
			 MVMAP2315_FLASH_SIZE, flash_mem);
}

void mvmap2315_mmu_init(void)
{
	mmu_init();

	mvmap2315_mmu_config();

	mmu_enable();
}
