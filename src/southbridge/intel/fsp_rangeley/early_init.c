/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2011 Google Inc
 * Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
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
 * Foundation, Inc.
 */

#include <stdint.h>
#include <stdlib.h>
#include <console/console.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <pc80/mc146818rtc.h>
#include <version.h>
#include <device/pci_def.h>
#include "pci_devs.h"
#include "soc.h"

static void rangeley_setup_bars(void)
{
	/* Setting up Southbridge. */
	printk(BIOS_DEBUG, "Setting up static southbridge registers...");
	pci_write_config32(LPC_BDF, RCBA, (uintptr_t)DEFAULT_RCBA | RCBA_ENABLE);
	pci_write_config32(LPC_BDF, ABASE, DEFAULT_ABASE | SET_BAR_ENABLE);
	pci_write_config32(LPC_BDF, PBASE, DEFAULT_PBASE | SET_BAR_ENABLE);
	printk(BIOS_DEBUG, " done.\n");

	printk(BIOS_DEBUG, "Disabling Watchdog timer...");
	/* Disable the watchdog reboot and turn off the watchdog timer */
	write8((void *)(DEFAULT_PBASE + PMC_CFG),
	       read8((void *)(DEFAULT_PBASE + PMC_CFG)) | NO_REBOOT);	// disable reboot on timer trigger
	outw(DEFAULT_ABASE + TCO1_CNT, inw(DEFAULT_ABASE + TCO1_CNT) |
		TCO_TMR_HALT);	// disable watchdog timer

	printk(BIOS_DEBUG, " done.\n");

}

static void reset_rtc(void)
{
	uint32_t pbase = pci_read_config32(LPC_BDF, PBASE) &
		0xfffffff0;
	uint32_t gen_pmcon1 = read32((void *)(pbase + GEN_PMCON1));
	int rtc_failed = !!(gen_pmcon1 & RPS);

	if (rtc_failed) {
		printk(BIOS_DEBUG,
			"RTC Failure detected.  Resetting Date to %s\n",
			coreboot_dmi_date);

		/* Clear the power failure flag */
		write32((void *)(DEFAULT_PBASE + GEN_PMCON1),
			gen_pmcon1 & ~RPS);
	}

	cmos_init(rtc_failed);
}

void rangeley_sb_early_initialization(void)
{
	/* Setup all BARs required for early PCIe and raminit */
	rangeley_setup_bars();

	reset_rtc();
}
