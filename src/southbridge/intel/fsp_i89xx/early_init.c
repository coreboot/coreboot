/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2011 Google Inc
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
#include <elog.h>
#include "pch.h"

static void sandybridge_setup_bars(void)
{
	/* Setting up Southbridge. */
	printk(BIOS_DEBUG, "Setting up static southbridge registers...");
	pci_write_config32(PCI_DEV(0, 0x1f, 0), RCBA, (uintptr_t)DEFAULT_RCBA | 1);

	pci_write_config32(PCI_DEV(0, 0x1f, 0), PMBASE, DEFAULT_PMBASE | 1);
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0x44 /* ACPI_CNTL */ , 0x80); /* Enable ACPI BAR */

	printk(BIOS_DEBUG, " done.\n");

	printk(BIOS_DEBUG, "Disabling Watchdog reboot...");
	RCBA32(GCS) = RCBA32(GCS) | (1 << 5);	/* No reset */
	outw((1 << 11), DEFAULT_PMBASE | 0x60 | 0x08);	/* halt timer */
	printk(BIOS_DEBUG, " done.\n");

#if CONFIG_ELOG_BOOT_COUNT
	/* Increment Boot Counter for non-S3 resume */
	if ((inw(DEFAULT_PMBASE + PM1_STS) & WAK_STS) &&
	    ((inl(DEFAULT_PMBASE + PM1_CNT) >> 10) & 7) != SLP_TYP_S3)
		boot_count_increment();
#endif

	printk(BIOS_DEBUG, " done.\n");

#if CONFIG_ELOG_BOOT_COUNT
	/* Increment Boot Counter except when resuming from S3 */
	if ((inw(DEFAULT_PMBASE + PM1_STS) & WAK_STS) &&
	    ((inl(DEFAULT_PMBASE + PM1_CNT) >> 10) & 7) == SLP_TYP_S3)
		return;
	boot_count_increment();
#endif
}

void sandybridge_sb_early_initialization(void)
{
	/* Setup all BARs required for early PCIe and raminit */
	sandybridge_setup_bars();
}

void early_pch_init(void)
{
	u8 reg8;

	// reset rtc power status
	reg8 = pci_read_config8(PCH_LPC_DEV, 0xa4);
	reg8 &= ~(1 << 2);
	pci_write_config8(PCH_LPC_DEV, 0xa4, reg8);
}
