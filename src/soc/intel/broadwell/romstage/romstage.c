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
#include <arch/cpu.h>
#include <arch/io.h>
#include <arch/cbfs.h>
#include <arch/early_variables.h>
#include <bootmode.h>
#include <compiler.h>
#include <console/console.h>
#include <cbfs.h>
#include <cbmem.h>
#include <cpu/x86/mtrr.h>
#include <elog.h>
#include <security/tpm/tspi.h>
#include <program_loading.h>
#include <romstage_handoff.h>
#include <stage_cache.h>
#include <timestamp.h>
#include <soc/me.h>
#include <soc/pei_data.h>
#include <soc/pm.h>
#include <soc/reset.h>
#include <soc/romstage.h>
#include <soc/spi.h>

/* Entry from cache-as-ram.inc. */
asmlinkage void *romstage_main(unsigned long bist,
				uint32_t tsc_low, uint32_t tsc_hi)
{
	struct romstage_params rp = {
		.bist = bist,
		.pei_data = NULL,
	};

	post_code(0x30);

	/* Save initial timestamp from bootblock. */
	timestamp_init((((uint64_t)tsc_hi) << 32) | (uint64_t)tsc_low);

	/* Save romstage begin */
	timestamp_add_now(TS_START_ROMSTAGE);

	/* System Agent Early Initialization */
	systemagent_early_init();

	/* PCH Early Initialization */
	pch_early_init();

	/* Call into mainboard pre console init. Needed to enable serial port
	   on IT8772 */
	mainboard_pre_console_init();

	/* Start console drivers */
	console_init();

	/* Get power state */
	rp.power_state = fill_power_state();

	/* Print useful platform information */
	report_platform_info();

	/* Set CPU frequency to maximum */
	set_max_freq();

	/* Call into mainboard. */
	mainboard_romstage_entry(&rp);

	return setup_stack_and_mtrrs();
}

/* Entry from the mainboard. */
void romstage_common(struct romstage_params *params)
{
	post_code(0x32);

	timestamp_add_now(TS_BEFORE_INITRAM);

	params->pei_data->boot_mode = params->power_state->prev_sleep_state;

#if IS_ENABLED(CONFIG_ELOG_BOOT_COUNT)
	if (params->power_state->prev_sleep_state != ACPI_S3)
		boot_count_increment();
#endif

	/* Print ME state before MRC */
	intel_me_status();

	/* Save ME HSIO version */
	intel_me_hsio_version(&params->power_state->hsio_version,
			      &params->power_state->hsio_checksum);

	/* Initialize RAM */
	raminit(params->pei_data);

	timestamp_add_now(TS_AFTER_INITRAM);

	romstage_handoff_init(params->power_state->prev_sleep_state == ACPI_S3);

	if (IS_ENABLED(CONFIG_TPM1) || IS_ENABLED(CONFIG_TPM2))
		tpm_setup(params->power_state->prev_sleep_state == ACPI_S3);
}

asmlinkage void romstage_after_car(void)
{
	/* Load the ramstage. */
	run_ramstage();
	while (1)
		;
}

void __weak mainboard_pre_console_init(void) {}
