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
#include <arch/exception.h>
#include <arch/stages.h>
#include <armv7.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <delay.h>
#include <program_loading.h>
#include <soc/gpio.h>
#include <soc/sdram.h>
#include <stdlib.h>
#include <symbols.h>
#include <timestamp.h>
#include <types.h>
#include <vendorcode/google/chromeos/chromeos.h>

void main(void)
{
	timestamp_add_now(TS_START_ROMSTAGE);

	console_init();

	timestamp_add_now(TS_BEFORE_INITRAM);
	sdram_init();
	timestamp_add_now(TS_AFTER_INITRAM);

	/* Now that DRAM is up, add mappings for it and DMA coherency buffer. */
	mmu_config_range((uintptr_t)_dram/MiB,
			 sdram_size_mb(), DCACHE_WRITEBACK);
	mmu_config_range((uintptr_t)_dma_coherent/MiB,
			 _dma_coherent_size/MiB, DCACHE_OFF);
	gpio_init();

	cbmem_initialize_empty();

	run_ramstage();
}
