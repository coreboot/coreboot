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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <fsp/car.h>
#include <soc/intel/common/util.h>
#include <timestamp.h>

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

	/* Ensure the EC is in the right mode for recovery */
	if (IS_ENABLED(CONFIG_EC_GOOGLE_CHROMEEC))
		google_chromeec_early_init();

	/* Return new stack value in ram back to assembly stub. */
	return cache_as_ram_stage_main(car_params->fih);
}

asmlinkage void after_cache_as_ram(void *chipset_context)
{
	timestamp_add_now(TS_FSP_TEMP_RAM_EXIT_END);
	printk(BIOS_DEBUG, "FspTempRamExit returned successfully\n");
	soc_display_mtrrs();

	after_cache_as_ram_stage();
}

void __attribute__((weak)) car_mainboard_pre_console_init(void)
{
}

void __attribute__((weak)) car_soc_pre_console_init(void)
{
}

void __attribute__((weak)) car_mainboard_post_console_init(void)
{
}

void __attribute__((weak)) car_soc_post_console_init(void)
{
}
