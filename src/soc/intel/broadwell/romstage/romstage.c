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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <stddef.h>
#include <stdint.h>
#include <arch/cpu.h>
#include <arch/io.h>
#include <arch/cbfs.h>
#include <arch/stages.h>
#include <arch/early_variables.h>
#include <console/console.h>
#include <cbfs.h>
#include <cbmem.h>
#include <cpu/x86/mtrr.h>
#include <elog.h>
#include <tpm.h>
#include <romstage_handoff.h>
#include <stage_cache.h>
#include <timestamp.h>
#include <soc/me.h>
#include <soc/pei_data.h>
#include <soc/pm.h>
#include <soc/reset.h>
#include <soc/romstage.h>
#include <soc/spi.h>
#include <vendorcode/google/chromeos/chromeos.h>

/* Entry from cache-as-ram.inc. */
void * asmlinkage romstage_main(unsigned long bist,
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

#if CONFIG_CHROMEOS
	save_chromeos_gpios();
#endif

	return setup_stack_and_mttrs();
}

/* Entry from the mainboard. */
void romstage_common(struct romstage_params *params)
{
	struct romstage_handoff *handoff;

	post_code(0x32);

	timestamp_add_now(TS_BEFORE_INITRAM);

	params->pei_data->boot_mode = params->power_state->prev_sleep_state;

#if CONFIG_ELOG_BOOT_COUNT
	if (params->power_state->prev_sleep_state != SLEEP_STATE_S3)
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

	handoff = romstage_handoff_find_or_add();
	if (handoff != NULL)
		handoff->s3_resume = (params->power_state->prev_sleep_state ==
				      SLEEP_STATE_S3);
	else
		printk(BIOS_DEBUG, "Romstage handoff structure not added!\n");

#if CONFIG_LPC_TPM
	init_tpm(params->power_state->prev_sleep_state == SLEEP_STATE_S3);
#endif
}

void asmlinkage romstage_after_car(void)
{
	/* Load the ramstage. */
	copy_and_run();
	while (1);
}

void ramstage_cache_invalid(void)
{
#if CONFIG_RESET_ON_INVALID_RAMSTAGE_CACHE
	/* Perform cold reset on invalid ramstage cache. */
	reset_system();
#endif
}

int get_sw_write_protect_state(void)
{
	u8 status;
	/* Return unprotected status if status read fails. */
	return (early_spi_read_wpsr(&status) ? 0 : !!(status & 0x80));
}

void __attribute__((weak)) mainboard_pre_console_init(void) {}
