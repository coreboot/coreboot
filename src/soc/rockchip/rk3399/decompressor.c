/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Rockchip Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <arch/io.h>
#include <arch/mmu.h>
#include <bootblock_common.h>
#include <console/console.h>
#include <soc/mmu_operations.h>
#include <symbols.h>

void decompressor_soc_init(void)
{
	mmu_init();

	/* Set 0x0 to max sdram(4GiB) supported by RK3399 as device memory.
	 * We want to configure mmio space(start at 0xf8000000) to DEV_MEM,
	 * some boards may use 2GB sdram in future(who knows).
	 */
	mmu_config_range((void *)0, (uintptr_t)4 * GiB, DEV_MEM);

	mmu_config_range(_sram, _sram_size, SECURE_MEM);

	mmu_enable();
}
