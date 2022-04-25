/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <stdint.h>
#include <console/console.h>
#include <device/pci_ops.h>
#include <timestamp.h>
#include <romstage_handoff.h>
#include "ironlake.h"
#include <arch/romstage.h>
#include <device/pci_def.h>
#include <device/device.h>
#include <northbridge/intel/ironlake/chip.h>
#include <northbridge/intel/ironlake/raminit.h>
#include <southbridge/intel/common/pmclib.h>
#include <southbridge/intel/ibexpeak/pch.h>
#include <southbridge/intel/ibexpeak/me.h>

/*
 * Platform has no romstage entry point under mainboard directory,
 * so this one is named with prefix mainboard.
 */
void mainboard_romstage_entry(void)
{
	int s3resume = 0;
	u8 spd_addrmap[4] = {};

	/* TODO, make this configurable */
	ironlake_early_initialization(IRONLAKE_MOBILE);

	early_pch_init();

	s3resume = southbridge_detect_s3_resume();
	if (s3resume) {
		u8 reg8 = pci_read_config8(PCI_DEV(0, 0x1f, 0), 0xa2);
		if (!(reg8 & 0x20)) {
			s3resume = 0;
			printk(BIOS_DEBUG, "Bad resume from S3 detected.\n");
		}
	}

	early_thermal_init();

	timestamp_add_now(TS_INITRAM_START);

	chipset_init(s3resume);

	mainboard_pre_raminit();

	mainboard_get_spd_map(spd_addrmap);

	raminit(s3resume, spd_addrmap);

	timestamp_add_now(TS_INITRAM_END);

	intel_early_me_status();

	romstage_handoff_init(s3resume);
}
