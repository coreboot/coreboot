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
	u8 reg8;

	// reset rtc power status
	reg8 = pci_read_config8(PCH_LPC_DEV, GEN_PMCON_3);
	reg8 &= ~(1 << 2);
	pci_write_config8(PCH_LPC_DEV, GEN_PMCON_3, reg8);
}

/* Platform has no romstage entry point under mainboard directory,
 * so this one is named with prefix mainboard.
 */
void mainboard_romstage_entry(void)
{
	int s3resume = 0;

	if (MCHBAR16(SSKPD) == 0xCAFE)
		system_reset();

	enable_lapic();

	/* Init LPC, GPIO, BARs, disable watchdog ... */
	early_pch_init();

	/* USB is initialized in MRC if MRC is used.  */
	if (CONFIG(USE_NATIVE_RAMINIT)) {
		early_usb_init(mainboard_usb_ports);
	}

	/* Perform some early chipset initialization required
	 * before RAM initialization can work
	 */
	systemagent_early_init();
	printk(BIOS_DEBUG, "Back from systemagent_early_init()\n");

	s3resume = southbridge_detect_s3_resume();

	elog_boot_notify(s3resume);

	post_code(0x38);

	mainboard_early_init(s3resume);

	post_code(0x39);

	perform_raminit(s3resume);

	timestamp_add_now(TS_AFTER_INITRAM);

	post_code(0x3b);
	/* Perform some initialization that must run before stage2 */
	early_pch_reset_pmcon();
	post_code(0x3c);

	southbridge_configure_default_intmap();
	southbridge_rcba_config();
	mainboard_late_rcba_config();

	post_code(0x3d);

	northbridge_romstage_finalize(s3resume);

	post_code(0x3f);
}
