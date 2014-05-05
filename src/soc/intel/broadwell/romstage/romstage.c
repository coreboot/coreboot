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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stddef.h>
#include <stdint.h>
#include <arch/cpu.h>
#include <arch/io.h>
#include <arch/cbfs.h>
#include <arch/stages.h>
#include <arch/early_variables.h>
#include <console/console.h>
#include <cbmem.h>
#include <cpu/x86/mtrr.h>
#include <elog.h>
#include <ramstage_cache.h>
#include <romstage_handoff.h>
#include <timestamp.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <broadwell/me.h>
#include <broadwell/pei_data.h>
#include <broadwell/pm.h>
#include <broadwell/reset.h>
#include <broadwell/romstage.h>
#include <broadwell/spi.h>

static inline void mark_ts(struct romstage_params *rp, uint64_t ts)
{
	struct romstage_timestamps *rt = &rp->ts;

	rt->times[rt->count] = ts;
	rt->count++;
}

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
	mark_ts(&rp, (((uint64_t)tsc_hi) << 32) | (uint64_t)tsc_low);

	/* Save romstage begin */
	mark_ts(&rp, timestamp_get());

	/* System Agent Early Initialization */
	systemagent_early_init();

	/* PCH Early Initialization */
	pch_early_init();

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

	return setup_stack_and_mttrs();
}

static inline void chromeos_init(int prev_sleep_state)
{
#if CONFIG_CHROMEOS
	/* Normalize the sleep state to what init_chromeos() wants for S3: 2 */
	init_chromeos(prev_sleep_state == SLEEP_STATE_S3 ? 2 : 0);
#endif
}

/* Entry from the mainboard. */
void romstage_common(struct romstage_params *params)
{
	struct romstage_handoff *handoff;

	post_code(0x32);

	mark_ts(params, timestamp_get());

	params->pei_data->boot_mode = params->power_state->prev_sleep_state;

#if CONFIG_ELOG_BOOT_COUNT
	if (params->power_state->prev_sleep_state != SLEEP_STATE_S3)
		boot_count_increment();
#endif

	/* Print ME state before MRC */
	intel_me_status();

	/* Initialize RAM */
	raminit(params->pei_data);
	mark_ts(params, timestamp_get());

	handoff = romstage_handoff_find_or_add();
	if (handoff != NULL)
		handoff->s3_resume = (params->power_state->prev_sleep_state ==
				      SLEEP_STATE_S3);
	else
		printk(BIOS_DEBUG, "Romstage handoff structure not added!\n");

	chromeos_init(params->power_state->prev_sleep_state);

	/* Save timestamp information. */
	timestamp_init(params->ts.times[0]);
	timestamp_add(TS_START_ROMSTAGE, params->ts.times[1]);
	timestamp_add(TS_BEFORE_INITRAM, params->ts.times[2]);
	timestamp_add(TS_AFTER_INITRAM, params->ts.times[3]);
}

void asmlinkage romstage_after_car(void)
{
	timestamp_add_now(TS_END_ROMSTAGE);

	/* Run vboot verification if configured. */
	vboot_verify_firmware(romstage_handoff_find_or_add());

	/* Load the ramstage. */
	copy_and_run();
	while (1);
}

void ramstage_cache_invalid(struct ramstage_cache *cache)
{
#if CONFIG_RESET_ON_INVALID_RAMSTAGE_CACHE
	/* Perform cold reset on invalid ramstage cache. */
	reset_system();
#endif
}

#if CONFIG_CHROMEOS
int vboot_get_sw_write_protect(void)
{
	u8 status;
	/* Return unprotected status if status read fails. */
	return (early_spi_read_wpsr(&status) ? 0 : !!(status & 0x80));
}
#endif
