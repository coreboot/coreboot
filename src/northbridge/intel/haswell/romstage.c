/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/romstage.h>
#include <console/console.h>
#include <device/mmio.h>
#include <elog.h>
#include <romstage_handoff.h>
#include <security/intel/txt/txt.h>
#include <security/intel/txt/txt_register.h>
#include <northbridge/intel/haswell/haswell.h>
#include <northbridge/intel/haswell/raminit.h>
#include <southbridge/intel/common/pmclib.h>
#include <southbridge/intel/lynxpoint/pch.h>

void __weak mb_late_romstage_setup(void)
{
}

/* The romstage entry point for this platform is not mainboard-specific, hence the name */
void mainboard_romstage_entry(void)
{
	early_pch_init();

	/* Perform some early chipset initialization required
	 * before RAM initialization can work
	 */
	haswell_early_initialization();
	printk(BIOS_DEBUG, "Back from haswell_early_initialization()\n");

	const int s3resume = southbridge_detect_s3_resume();

	elog_boot_notify(s3resume);

	/* Prepare USB controller early in S3 resume */
	if (s3resume)
		enable_usb_bar();

	post_code(0x3a);

	report_platform_info();

	if (CONFIG(INTEL_TXT))
		intel_txt_romstage_init();

	perform_raminit(s3resume);

	if (CONFIG(INTEL_TXT)) {
		printk(BIOS_DEBUG, "Check TXT_ERROR register after MRC\n");

		intel_txt_log_acm_error(read32p(TXT_ERROR));

		intel_txt_log_spad();

		intel_txt_memory_has_secrets();

		txt_dump_regions();
	}

	haswell_unhide_peg();

	romstage_handoff_init(s3resume);

	mb_late_romstage_setup();

	post_code(0x3f);
}
