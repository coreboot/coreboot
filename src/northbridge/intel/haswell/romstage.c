/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/romstage.h>
#include <console/console.h>
#include <cf9_reset.h>
#include <timestamp.h>
#include <cpu/x86/lapic.h>
#include <cbmem.h>
#include <commonlib/helpers.h>
#include <romstage_handoff.h>
#include <cpu/intel/haswell/haswell.h>
#include <northbridge/intel/haswell/haswell.h>
#include <northbridge/intel/haswell/raminit.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <southbridge/intel/lynxpoint/me.h>

/* Copy SPD data for on-board memory */
void __weak copy_spd(struct pei_data *peid)
{
}

void __weak mb_late_romstage_setup(void)
{
}

/* The romstage entry point for this platform is not mainboard-specific, hence the name */
void mainboard_romstage_entry(void)
{
	int wake_from_s3;

	struct pei_data pei_data = {
	};

	mainboard_fill_pei_data(&pei_data);

	enable_lapic();

	wake_from_s3 = early_pch_init();

	/* Perform some early chipset initialization required
	 * before RAM initialization can work
	 */
	haswell_early_initialization();
	printk(BIOS_DEBUG, "Back from haswell_early_initialization()\n");

	if (wake_from_s3) {
#if CONFIG(HAVE_ACPI_RESUME)
		printk(BIOS_DEBUG, "Resume from S3 detected.\n");
#else
		printk(BIOS_DEBUG, "Resume from S3 detected, but disabled.\n");
		wake_from_s3 = 0;
#endif
	}

	/* Prepare USB controller early in S3 resume */
	if (wake_from_s3)
		enable_usb_bar();

	post_code(0x3a);

	/* MRC has hardcoded assumptions of 2 meaning S3 wake. Normalize it here. */
	pei_data.boot_mode = wake_from_s3 ? 2 : 0;

	timestamp_add_now(TS_BEFORE_INITRAM);

	report_platform_info();

	copy_spd(&pei_data);

	sdram_initialize(&pei_data);

	timestamp_add_now(TS_AFTER_INITRAM);

	post_code(0x3b);

	intel_early_me_status();

	if (!wake_from_s3) {
		cbmem_initialize_empty();
		/* Save data returned from MRC on non-S3 resumes. */
		save_mrc_data(&pei_data);
	} else if (cbmem_initialize()) {
	#if CONFIG(HAVE_ACPI_RESUME)
		/* Failed S3 resume, reset to come up cleanly */
		system_reset();
	#endif
	}

	haswell_unhide_peg();

	setup_sdram_meminfo(&pei_data);

	romstage_handoff_init(wake_from_s3);

	mb_late_romstage_setup();

	post_code(0x3f);
}
