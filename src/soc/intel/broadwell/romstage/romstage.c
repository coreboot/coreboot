/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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
#include <arch/cbfs.h>
#include <bootblock_common.h>
#include <bootmode.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/intel/romstage.h>
#include <cpu/x86/mtrr.h>
#include <elog.h>
#include <program_loading.h>
#include <romstage_handoff.h>
#include <stage_cache.h>
#include <timestamp.h>
#include <soc/gpio.h>
#include <soc/me.h>
#include <soc/pei_data.h>
#include <soc/pm.h>
#include <soc/romstage.h>
#include <soc/spi.h>

#define ROMSTAGE_RAM_STACK_SIZE 0x5000

/* platform_enter_postcar() determines the stack to use after
 * cache-as-ram is torn down as well as the MTRR settings to use,
 * and continues execution in postcar stage. */
void platform_enter_postcar(void)
{
	struct postcar_frame pcf;
	uintptr_t top_of_ram;

	if (postcar_frame_init(&pcf, ROMSTAGE_RAM_STACK_SIZE))
		die("Unable to initialize postcar frame.\n");
	/* Cache the ROM as WP just below 4GiB. */
	postcar_frame_add_romcache(&pcf, MTRR_TYPE_WRPROT);

	/* Cache RAM as WB from 0 -> CACHE_TMP_RAMTOP. */
	postcar_frame_add_mtrr(&pcf, 0, CACHE_TMP_RAMTOP, MTRR_TYPE_WRBACK);

	/* Cache at least 8 MiB below the top of ram, and at most 8 MiB
	 * above top of the ram. This satisfies MTRR alignment requirement
	 * with different TSEG size configurations.
	 */
	top_of_ram = ALIGN_DOWN((uintptr_t)cbmem_top(), 8*MiB);
	postcar_frame_add_mtrr(&pcf, top_of_ram - 8*MiB, 16*MiB,
			MTRR_TYPE_WRBACK);

	run_postcar_phase(&pcf);
}

/* Entry from cpu/intel/car/romstage.c. */
void mainboard_romstage_entry(unsigned long bist)
{
	struct romstage_params rp = { 0 };

	post_code(0x30);

	/* System Agent Early Initialization */
	systemagent_early_init();

	/* PCH Early Initialization */
	pch_early_init();

	/* Get power state */
	rp.power_state = fill_power_state();

	/* Print useful platform information */
	report_platform_info();

	/* Set CPU frequency to maximum */
	set_max_freq();

	/* Initialize GPIOs */
	init_gpios(mainboard_gpio_config);

	/* Fill in mainboard pei_date. */
	mainboard_pre_raminit(&rp);

	post_code(0x32);

	timestamp_add_now(TS_BEFORE_INITRAM);

	rp.pei_data.boot_mode = rp.power_state->prev_sleep_state;

	if (CONFIG(ELOG_BOOT_COUNT)
			&& rp.power_state->prev_sleep_state != ACPI_S3)
		boot_count_increment();

	/* Print ME state before MRC */
	intel_me_status();

	/* Save ME HSIO version */
	intel_me_hsio_version(&rp.power_state->hsio_version,
			      &rp.power_state->hsio_checksum);

	/* Initialize RAM */
	raminit(&rp.pei_data);

	timestamp_add_now(TS_AFTER_INITRAM);

	romstage_handoff_init(rp.power_state->prev_sleep_state == ACPI_S3);

	mainboard_post_raminit(&rp);
}
