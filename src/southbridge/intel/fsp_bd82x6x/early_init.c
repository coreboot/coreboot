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
 */

#include <stdint.h>
#include <stdlib.h>
#include <console/console.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <elog.h>
#include "pch.h"

#define FD_ENTRIES	32
#define FD2_ENTRIES	5

const static char *fd_set_strings[FD_ENTRIES] = {
	"",
	"PCI Bridge (D30:F0) Disabled\n",
	"SATA 1(D31:F2) Disabled\n",
	"SMBus Config space Disabled\n",
	"High Definition Audio Disabled\n",
	"Reserved bit 5 set\n",
	"Reserved bit 6 set\n",
	"Reserved bit 7 set\n",
	"Reserved bit 8 set\n",
	"Reserved bit 9 set\n",
	"Reserved bit 10 set\n",
	"Reserved bit 11 set\n",
	"Reserved bit 12 set\n",
	"EHCI #2 Disabled\n",
	"LPC Bridge Disabled\n",
	"EHCI #1 Disabled\n",
	"PCIe bridge 1 Disabled\n",
	"PCIe bridge 2 Disabled\n",
	"PCIe bridge 3 Disabled\n",
	"PCIe bridge 4 Disabled\n",
	"PCIe bridge 5 Disabled\n",
	"PCIe bridge 6 Disabled\n",
	"PCIe bridge 7 Disabled\n",
	"PCIe bridge 8 Disabled\n",
	"Thermal Sensor (D31:F6) Registers Disabled\n",
	"SATA 2 (D31:F5) Disabled\n",
	"Reserved bit 26 set\n",
	"Reserved bit 27 set\n",
	"Reserved bit 28 set\n",
	"Reserved bit 29 set\n",
	"Reserved bit 30 set\n",
	"Reserved bit 31 set\n",
};

const static char *fd_notset_strings[FD_ENTRIES] = {
	"ERROR: Required field NOT programmed\n",
	"PCI Bridge (D30:F0) enabled\n",
	"SATA 1(D31:F2) enabled\n",
	"SMBus Config space enabled\n",
	"High Definition Audio enabled\n",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"EHCI #2 Enabled\n",
	"LPC Bridge Enabled\n",
	"EHCI #1 Enabled\n",
	"PCIe bridge 1 Enabled\n",
	"PCIe bridge 2 Enabled\n",
	"PCIe bridge 3 Enabled\n",
	"PCIe bridge 4 Enabled\n",
	"PCIe bridge 5 Enabled\n",
	"PCIe bridge 6 Enabled\n",
	"PCIe bridge 7 Enabled\n",
	"PCIe bridge 8 Enabled\n",
	"Thermal Sensor (D31:F6) Registers Enabled\n",
	"SATA 2 (D31:F5) Enabled\n",
	"",
	"",
	"",
	"",
	"",
	"",
};

const static char *fd2_set_strings[FD2_ENTRIES] = {
	"Display BDF Enabled\n",
	"MEI #1 (D22:F0) Disabled\n",
	"MEI #2 (D22:F1) Disabled\n",
	"IDE-R (D22:F2) Disabled\n",
	"KT (D22:F3) Disabled\n"
};

const static char *fd2_notset_strings[FD2_ENTRIES] = {
	"Display BDF Disabled\n",
	"MEI #1 (D22:F0) Enabled\n",
	"MEI #2 (D22:F1) Enabled\n",
	"IDE-R (D22:F2) Enabled\n",
	"KT (D22:F3) Enabled\n"
};

void display_fd_settings(void)
{
	u32 reg32;
	int i;

	reg32 = RCBA32(FD);
	for (i = 0; i < FD_ENTRIES; i++) {
		if (reg32 & (1 << i)) {
			printk(BIOS_SPEW, "%s", fd_set_strings[i]);
		} else {
			printk(BIOS_SPEW, "%s", fd_notset_strings[i]);
		}
	}

	reg32 = RCBA32(FD2);
	for (i = 0; i < FD2_ENTRIES; i++) {
		if (reg32 & (1 << i)) {
			printk(BIOS_SPEW, "%s", fd2_set_strings[i]);
		} else {
			printk(BIOS_SPEW, "%s", fd2_notset_strings[i]);
		}
	}
}

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
