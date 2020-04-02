/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <stdint.h>
#include <console/console.h>
#include <cf9_reset.h>
#include <cpu/x86/bist.h>
#include <timestamp.h>
#include <cpu/x86/lapic.h>
#include <cbmem.h>
#include <commonlib/helpers.h>
#include <romstage_handoff.h>
#include <northbridge/intel/haswell/haswell.h>
#include <northbridge/intel/haswell/raminit.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <southbridge/intel/lynxpoint/me.h>
#include "haswell.h"

void romstage_common(const struct romstage_params *params)
{
	int boot_mode;
	int wake_from_s3;

	enable_lapic();

	wake_from_s3 = early_pch_init(params->gpio_map, params->rcba_config);

	/* Perform some early chipset initialization required
	 * before RAM initialization can work
	 */
	haswell_early_initialization(HASWELL_MOBILE);
	printk(BIOS_DEBUG, "Back from haswell_early_initialization()\n");

	if (wake_from_s3) {
#if CONFIG(HAVE_ACPI_RESUME)
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

	if (!wake_from_s3) {
		cbmem_initialize_empty();
		/* Save data returned from MRC on non-S3 resumes. */
		save_mrc_data(params->pei_data);
	} else if (cbmem_initialize()) {
	#if CONFIG(HAVE_ACPI_RESUME)
		/* Failed S3 resume, reset to come up cleanly */
		system_reset();
	#endif
	}

	haswell_unhide_peg();

	setup_sdram_meminfo(params->pei_data);

	romstage_handoff_init(wake_from_s3);

	post_code(0x3f);
}
