/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
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

#include <arch/cache.h>
#include <bootblock_common.h>
#include <stddef.h>
#include <symbols.h>
#include <soc/hw_init.h>

void bootblock_soc_init(void)
{
	/*
	 * not only for speed but for preventing the cpu from crashing.
	 * the cpu is not happy when cache is cleaned without mmu turned on.
	 */
	mmu_init();
	mmu_config_range(0, 4096, DCACHE_OFF);
	mmu_config_range_kb((uintptr_t)_sram/KiB, _sram_size/KiB,
			    DCACHE_WRITETHROUGH);
	dcache_mmu_enable();

	hw_init();
}
