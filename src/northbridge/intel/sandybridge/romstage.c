/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cf9_reset.h>
#include <device/pci_ops.h>
#include <cpu/x86/lapic.h>
#include <romstage_handoff.h>
#include "sandybridge.h"
#include <arch/romstage.h>
#include <device/pci_def.h>
#include <device/device.h>
#include <northbridge/intel/sandybridge/chip.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/pmclib.h>
#include <elog.h>

__weak void mainboard_early_init(int s3_resume)
{
}

__weak void mainboard_late_rcba_config(void)
{
}

static void early_pch_reset_pmcon(void)
{
	/* Reset RTC power status */
	pci_and_config8(PCH_LPC_DEV, GEN_PMCON_3, ~(1 << 2));
}

/* The romstage entry point for this platform is not mainboard-specific, hence the name */
void mainboard_romstage_entry(void)
{
	int s3resume = 0;

	if (MCHBAR16(SSKPD_HI) == 0xCAFE)
		system_reset();

	enable_lapic();

	/* Init LPC, GPIO, BARs, disable watchdog ... */
	early_pch_init();

	/* When using MRC, USB is initialized by MRC */
	if (CONFIG(USE_NATIVE_RAMINIT)) {
		early_usb_init(mainboard_usb_ports);
	}

	/* Perform some early chipset init needed before RAM initialization can work */
	systemagent_early_init();
	printk(BIOS_DEBUG, "Back from systemagent_early_init()\n");

	s3resume = southbridge_detect_s3_resume();

	elog_boot_notify(s3resume);

	post_code(0x38);

	mainboard_early_init(s3resume);

	post_code(0x39);

	perform_raminit(s3resume);

	post_code(0x3b);
	/* Perform some initialization that must run before stage2 */
	early_pch_reset_pmcon();
	post_code(0x3c);

	southbridge_configure_default_intmap();
	southbridge_rcba_config();
	mainboard_late_rcba_config();

	post_code(0x3d);

	northbridge_romstage_finalize();

	post_code(0x3f);

	romstage_handoff_init(s3resume);
}
