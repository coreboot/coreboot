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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <types.h>
#include <armv7.h>
#include <cbfs.h>
#include <console/console.h>
#include <arch/stages.h>
#include <cbmem.h>
#include <delay.h>
#include <timestamp.h>
#include <arch/cache.h>
#include <arch/exception.h>

void main(void)
{
	void *entry;

	console_init();

	/* used for MMU and CBMEM setup, in MB */
	u32 dram_start = (CONFIG_SYS_SDRAM_BASE >> 20);
	u32 dram_size = CONFIG_DRAM_SIZE_MB;
	u32 dram_end = dram_start + dram_size;
	mmu_init();
	/* Device memory below DRAM is uncached. */
	mmu_config_range(0, dram_start, DCACHE_OFF);
	/* DRAM is cached. */
	mmu_config_range(dram_start, dram_size, DCACHE_WRITEBACK);
	/* A window for DMA is uncached. */
	mmu_config_range(CONFIG_DRAM_DMA_START >> 20,
			 CONFIG_DRAM_DMA_SIZE >> 20, DCACHE_OFF);
	/* The space above DRAM is uncached. */
	if (dram_end < 4096)
		mmu_config_range(dram_end, 4096 - dram_end, DCACHE_OFF);
	mmu_disable_range(0, 1);
	dcache_mmu_enable();

	cbmem_initialize_empty();

	entry = cbfs_load_stage(CBFS_DEFAULT_MEDIA, "fallback/ramstage");
	stage_exit(entry);
}
