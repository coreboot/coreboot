/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Rockchip Inc.
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
#include <arch/io.h>
#include <bootblock_common.h>
#include <console/console.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/grf.h>
#include <soc/timer.h>
#include <symbols.h>
#include <timestamp.h>

void bootblock_soc_init(void)
{
	timestamp_init(timestamp_get());

	rkclk_init();

	mmu_init();
	/* Start with a clean slate. */
	mmu_config_range(0, 4096, DCACHE_OFF);
	/* SRAM is tightly wedged between registers, need to use subtables. Map
	 * write-through as equivalent for non-cacheable without XN on A17. */
	mmu_config_range_kb((uintptr_t)_sram/KiB,
			    _sram_size/KiB, DCACHE_WRITETHROUGH);
	dcache_mmu_enable();

	rkclk_configure_crypto(148500*KHz);
}
