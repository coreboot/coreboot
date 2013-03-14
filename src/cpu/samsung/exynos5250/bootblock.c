/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 The Chromium OS Authors
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <armv7.h>

/*
 * cortex_a15_cache_init - Initialize caches for Cortex-A15
 * L1 cache features:
 * - 32KB 2-way set associative icache, fixed 64-byte line length.
 * - 32KB 2-way set associative dcache, fixed 64-byte line length.
 *
 * L2 cache features:
 * - Unified, 16-way set associative, fixed 64-byte line length
 * - Configurable for 512KB, 1M, 2M, or 4MB sizes (SoC-dependent)
 *
 * FIXME: move this elsewhere 
 */
static void cortex_a15_cache_init(uint32_t dram_start, uint32_t dram_size)
{
	/* Disable caches */
	/* Clean and invalidate caches */
	/* Set L1 cache parameters */
	/* Enable L1 cache */
	/* Set L2 cache parameters */
	/* Enable L2 cache */
}

void bootblock_cpu_init(void);
void bootblock_cpu_init(void)
{
	cortex_a15_cache_init();

	/* Enable L2 cache */
	/* FIXME: maybe do this in romstage along with the other cache
	 * init functions? */
//	v7_outer_cache_enable();
}
