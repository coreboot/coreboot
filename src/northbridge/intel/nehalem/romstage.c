/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 * Copyright (C) 2014 Vladimir Serbinenko
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
#include <console/console.h>
#include <cf9_reset.h>
#include <device/pci_ops.h>
#include <cpu/x86/lapic.h>
#include <timestamp.h>
#include <romstage_handoff.h>
#include "nehalem.h"
#include <arch/romstage.h>
#include <device/pci_def.h>
#include <device/device.h>
#include <northbridge/intel/nehalem/chip.h>
#include <northbridge/intel/nehalem/raminit.h>
#include <southbridge/intel/ibexpeak/pch.h>
#include <southbridge/intel/ibexpeak/me.h>

/* Platform has no romstage entry point under mainboard directory,
 * so this one is named with prefix mainboard.
 */
void mainboard_romstage_entry(void)
{
	u32 reg32;
	int s3resume = 0;
	u8 spd_addrmap[4] = {};

	enable_lapic();

	/* TODO, make this configurable */
	nehalem_early_initialization(NEHALEM_MOBILE);

	early_pch_init();

	/* Initialize console device(s) */
	console_init();

	/* Read PM1_CNT, DON'T CLEAR IT or raminit will fail! */
	reg32 = inl(DEFAULT_PMBASE + 0x04);
	printk(BIOS_DEBUG, "PM1_CNT: %08x\n", reg32);
	if (((reg32 >> 10) & 7) == 5) {
		u8 reg8;
		reg8 = pci_read_config8(PCI_DEV(0, 0x1f, 0), 0xa2);
		printk(BIOS_DEBUG, "a2: %02x\n", reg8);
		if (!(reg8 & 0x20)) {
			outl(reg32 & ~(7 << 10), DEFAULT_PMBASE + 0x04);
			printk(BIOS_DEBUG, "Bad resume from S3 detected.\n");
		} else {
			if (acpi_s3_resume_allowed()) {
				printk(BIOS_DEBUG, "Resume from S3 detected.\n");
				s3resume = 1;
			} else {
				printk(BIOS_DEBUG,
				       "Resume from S3 detected, but disabled.\n");
			}
		}
	}

	/* Enable SMBUS. */
	enable_smbus();

	early_thermal_init();

	timestamp_add_now(TS_BEFORE_INITRAM);

	chipset_init(s3resume);

	mainboard_pre_raminit();

	mainboard_get_spd_map(spd_addrmap);

	raminit(s3resume, spd_addrmap);

	timestamp_add_now(TS_AFTER_INITRAM);

	intel_early_me_status();

	if (s3resume) {
		/* Clear SLP_TYPE. This will break stage2 but
		 * we care for that when we get there.
		 */
		reg32 = inl(DEFAULT_PMBASE + 0x04);
		outl(reg32 & ~(7 << 10), DEFAULT_PMBASE + 0x04);
	}

	romstage_handoff_init(s3resume);
}
