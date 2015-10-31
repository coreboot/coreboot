/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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
#include <soc/sdram.h>
#include <stdlib.h>
#include <symbols.h>
#include <timestamp.h>
#include <types.h>
#include <vendorcode/google/chromeos/chromeos.h>

#include "timer.h"

void main(void)
{
	timestamp_add_now(TS_START_ROMSTAGE);

	console_init();

	timestamp_add_now(TS_BEFORE_INITRAM);

	sdram_init();

	timestamp_add_now(TS_AFTER_INITRAM);

	mmu_init();
	mmu_config_range(0, 4096, DCACHE_OFF);
	dcache_mmu_enable();

	cbmem_initialize_empty();

	run_ramstage();
}
