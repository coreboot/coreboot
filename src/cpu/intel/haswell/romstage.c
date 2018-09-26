/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google LLC
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

#include <stdint.h>
#include <string.h>
#include <cbfs.h>
#include <console/console.h>
#include <arch/cpu.h>
#include <cpu/cpu.h>
#include <cpu/x86/bist.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <halt.h>
#include <lib.h>
#include <timestamp.h>
#include <arch/acpi.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <cpu/x86/lapic.h>
#include <cbmem.h>
#include <program_loading.h>
#include <romstage_handoff.h>
#include <reset.h>
#include <vendorcode/google/chromeos/chromeos.h>
#if IS_ENABLED(CONFIG_EC_GOOGLE_CHROMEEC)
#include <ec/google/chromeec/ec.h>
#endif
#include <northbridge/intel/haswell/haswell.h>
#include <northbridge/intel/haswell/raminit.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <southbridge/intel/lynxpoint/me.h>
#include <cpu/intel/romstage.h>
#include "haswell.h"

static inline void reset_system(void)
{
	hard_reset();
	halt();
}

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
	postcar_frame_add_mtrr(&pcf, CACHE_ROM_BASE, CACHE_ROM_SIZE,
		MTRR_TYPE_WRPROT);

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

void romstage_common(const struct romstage_params *params)
{
	int boot_mode;
	int wake_from_s3;

	timestamp_init(get_initial_timestamp());
	timestamp_add_now(TS_START_ROMSTAGE);

	if (params->bist == 0)
		enable_lapic();

	wake_from_s3 = early_pch_init(params->gpio_map, params->rcba_config);

	/* Halt if there was a built in self test failure */
	report_bist_failure(params->bist);

	/* Perform some early chipset initialization required
	 * before RAM initialization can work
	 */
	haswell_early_initialization(HASWELL_MOBILE);
	printk(BIOS_DEBUG, "Back from haswell_early_initialization()\n");

	if (wake_from_s3) {
#if IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)
		printk(BIOS_DEBUG, "Resume from S3 detected.\n");
#else
		printk(BIOS_DEBUG, "Resume from S3 detected, but disabled.\n");
		wake_from_s3 = 0;
#endif
	}

	/* There are hard coded assumptions of 2 meaning s3 wake. Normalize
	 * the users of the 2 literal here based off wake_from_s3. */
	boot_mode = wake_from_s3 ? 2 : 0;

	/* Prepare USB controller early in S3 resume */
	if (wake_from_s3)
		enable_usb_bar();

	post_code(0x3a);
	params->pei_data->boot_mode = boot_mode;

	timestamp_add_now(TS_BEFORE_INITRAM);

	report_platform_info();

	if (params->copy_spd != NULL)
		params->copy_spd(params->pei_data);

	sdram_initialize(params->pei_data);

	timestamp_add_now(TS_AFTER_INITRAM);

	post_code(0x3b);

	intel_early_me_status();

	quick_ram_check();
	post_code(0x3e);

	if (!wake_from_s3) {
		cbmem_initialize_empty();
		/* Save data returned from MRC on non-S3 resumes. */
		save_mrc_data(params->pei_data);
	} else if (cbmem_initialize()) {
	#if IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)
		/* Failed S3 resume, reset to come up cleanly */
		reset_system();
	#endif
	}

	setup_sdram_meminfo(params->pei_data);

	romstage_handoff_init(wake_from_s3);

	post_code(0x3f);
}
