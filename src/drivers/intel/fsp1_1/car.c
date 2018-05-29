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

#include <arch/early_variables.h>
#include <compiler.h>
#include <console/console.h>
#include <fsp/car.h>
#include <fsp/util.h>
#include <program_loading.h>
#include <soc/intel/common/util.h>
#include <timestamp.h>

FSP_INFO_HEADER *fih_car CAR_GLOBAL;

/* Save FSP_INFO_HEADER for TempRamExit() call in assembly. */
static inline void set_fih_car(FSP_INFO_HEADER *fih)
{
	/* This variable is written in the raw form because it's only
	 * ever accessed in code that that has the cache-as-ram enabled. The
	 * assembly routine which tears down cache-as-ram utilizes this
	 * variable for determining where to find FSP. */
	fih_car = fih;
}

asmlinkage void *cache_as_ram_main(struct cache_as_ram_params *car_params)
{
	/* Initialize timestamp book keeping only once. */
	timestamp_init(car_params->tsc);

	/* Call into pre-console init code then initialize console. */
	car_soc_pre_console_init();
	car_mainboard_pre_console_init();
	console_init();

	printk(BIOS_DEBUG, "FSP TempRamInit successful\n");

	printk(BIOS_SPEW, "bist: 0x%08x\n", car_params->bist);
	printk(BIOS_SPEW, "tsc: 0x%016llx\n", car_params->tsc);

	if (car_params->bootloader_car_start != CONFIG_DCACHE_RAM_BASE ||
	    car_params->bootloader_car_end !=
			(CONFIG_DCACHE_RAM_BASE + CONFIG_DCACHE_RAM_SIZE)) {
		printk(BIOS_INFO, "CAR mismatch: %08x--%08x vs %08lx--%08lx\n",
			CONFIG_DCACHE_RAM_BASE,
			CONFIG_DCACHE_RAM_BASE + CONFIG_DCACHE_RAM_SIZE,
			(long)car_params->bootloader_car_start,
			(long)car_params->bootloader_car_end);
	}

	car_soc_post_console_init();
	car_mainboard_post_console_init();

	set_fih_car(car_params->fih);

	/* Return new stack value in RAM back to assembly stub. */
	return cache_as_ram_stage_main(car_params->fih);
}

/* Entry point taken when romstage is called after a separate verstage. */
asmlinkage void *romstage_c_entry(void)
{
	/* Need to locate the current FSP_INFO_HEADER. The cache-as-ram
	 * is still enabled. We can directly access work buffer here. */
	FSP_INFO_HEADER *fih;
	struct prog fsp = PROG_INIT(PROG_REFCODE, "fsp.bin");

	console_init();

	if (prog_locate(&fsp)) {
		fih = NULL;
		printk(BIOS_ERR, "Unable to locate %s\n", prog_name(&fsp));
	} else
		/* This leaks a mapping which this code assumes is benign as
		 * the flash is memory mapped CPU's address space. */
		fih = find_fsp((uintptr_t)rdev_mmap_full(prog_rdev(&fsp)));

	set_fih_car(fih);

	/* Return new stack value in RAM back to assembly stub. */
	return cache_as_ram_stage_main(fih);
}

asmlinkage void after_cache_as_ram(void *chipset_context)
{
	timestamp_add_now(TS_FSP_TEMP_RAM_EXIT_END);
	printk(BIOS_DEBUG, "FspTempRamExit returned successfully\n");
	soc_display_mtrrs();

	after_cache_as_ram_stage();
}

void __weak car_mainboard_pre_console_init(void)
{
}

void __weak car_soc_pre_console_init(void)
{
}

void __weak car_mainboard_post_console_init(void)
{
}

void __weak car_soc_post_console_init(void)
{
}
