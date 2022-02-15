/* SPDX-License-Identifier: GPL-2.0-only */

#include <timestamp.h>
#include <console/console.h>
#include <device/pci_ops.h>
#include <cbmem.h>
#include <cf9_reset.h>
#include <romstage_handoff.h>
#include <southbridge/intel/i82801gx/i82801gx.h>
#include <southbridge/intel/common/pmclib.h>
#include <arch/romstage.h>
#include "raminit.h"
#include "pineview.h"

static void rcba_config(void)
{
	/* Set up Virtual Channel 0 */
	RCBA32(0x0014) = 0x80000001;
	RCBA32(0x001c) = 0x03128010;
}

__weak void mb_pirq_setup(void)
{
}

/* The romstage entry point for this platform is not mainboard-specific, hence the name. */
void mainboard_romstage_entry(void)
{
	u8 spd_addrmap[4] = {};
	int boot_path, cbmem_was_initted;
	int s3resume = 0;

	/* Do some early chipset init, necessary for RAM init to work */
	i82801gx_early_init();
	pineview_early_init();

	post_code(0x30);

	s3resume = southbridge_detect_s3_resume();

	if (s3resume) {
		boot_path = BOOT_PATH_RESUME;
	} else {
		if (mchbar_read32(PMSTS) & (1 << 8)) /* HOT RESET */
			boot_path = BOOT_PATH_RESET;
		else
			boot_path = BOOT_PATH_NORMAL;
	}

	get_mb_spd_addrmap(&spd_addrmap[0]);

	printk(BIOS_DEBUG, "Initializing memory\n");
	timestamp_add_now(TS_INITRAM_START);
	sdram_initialize(boot_path, spd_addrmap);
	timestamp_add_now(TS_INITRAM_END);
	printk(BIOS_DEBUG, "Memory initialized\n");

	post_code(0x31);

	mb_pirq_setup();

	rcba_config();

	cbmem_was_initted = !cbmem_recovery(s3resume);

	if (!cbmem_was_initted && s3resume) {
		/* Failed S3 resume, reset to come up cleanly */
		system_reset();
	}

	romstage_handoff_init(s3resume);
}
