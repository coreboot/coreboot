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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdint.h>
#include <stdlib.h>
#include <console/console.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <elog.h>
#include "haswell.h"

static void haswell_setup_bars(void)
{
	printk(BIOS_DEBUG, "Setting up static northbridge registers...");
	/* Set up all hardcoded northbridge BARs */
	pci_write_config32(PCI_DEV(0, 0x00, 0), EPBAR, DEFAULT_EPBAR | 1);
	pci_write_config32(PCI_DEV(0, 0x00, 0), EPBAR + 4, (0LL+DEFAULT_EPBAR) >> 32);
	pci_write_config32(PCI_DEV(0, 0x00, 0), MCHBAR, DEFAULT_MCHBAR | 1);
	pci_write_config32(PCI_DEV(0, 0x00, 0), MCHBAR + 4, (0LL+DEFAULT_MCHBAR) >> 32);
	pci_write_config32(PCI_DEV(0, 0x00, 0), DMIBAR, DEFAULT_DMIBAR | 1);
	pci_write_config32(PCI_DEV(0, 0x00, 0), DMIBAR + 4, (0LL+DEFAULT_DMIBAR) >> 32);

	/* Set C0000-FFFFF to access RAM on both reads and writes */
	pci_write_config8(PCI_DEV(0, 0x00, 0), PAM0, 0x30);
	pci_write_config8(PCI_DEV(0, 0x00, 0), PAM1, 0x33);
	pci_write_config8(PCI_DEV(0, 0x00, 0), PAM2, 0x33);
	pci_write_config8(PCI_DEV(0, 0x00, 0), PAM3, 0x33);
	pci_write_config8(PCI_DEV(0, 0x00, 0), PAM4, 0x33);
	pci_write_config8(PCI_DEV(0, 0x00, 0), PAM5, 0x33);
	pci_write_config8(PCI_DEV(0, 0x00, 0), PAM6, 0x33);

	printk(BIOS_DEBUG, " done.\n");
}

static void haswell_setup_graphics(void)
{
	u32 reg32;
	u16 reg16;
	u8 reg8;

	printk(BIOS_DEBUG, "Initializing Graphics...\n");

	/* Setup IGD memory by setting GGC[7:3] = 1 for 32MB */
	reg16 = pci_read_config16(PCI_DEV(0,0,0), GGC);
	reg16 &= ~0x00f8;
	reg16 |= 1 << 3;
	/* Program GTT memory by setting GGC[9:8] = 2MB */
	reg16 &= ~0x0300;
	reg16 |= 2 << 8;
	/* Enable VGA decode */
	reg16 &= ~0x0002;
	pci_write_config16(PCI_DEV(0,0,0), GGC, reg16);

	/* Enable 256MB aperture */
	reg8 = pci_read_config8(PCI_DEV(0, 2, 0), MSAC);
	reg8 &= ~0x06;
	reg8 |= 0x02;
	pci_write_config8(PCI_DEV(0, 2, 0), MSAC, reg8);

	/* Erratum workarounds */
	reg32 = MCHBAR32(0x5f00);
	reg32 |= (1 << 9)|(1 << 10);
	MCHBAR32(0x5f00) = reg32;

	/* Enable SA Clock Gating */
	reg32 = MCHBAR32(0x5f00);
	MCHBAR32(0x5f00) = reg32 | 1;

	/* GPU RC6 workaround for sighting 366252 */
	reg32 = MCHBAR32(0x5d14);
	reg32 |= (1 << 31);
	MCHBAR32(0x5d14) = reg32;

	/* VLW */
	reg32 = MCHBAR32(0x6120);
	reg32 &= ~(1 << 0);
	MCHBAR32(0x6120) = reg32;

	reg32 = MCHBAR32(0x5418);
	reg32 |= (1 << 4) | (1 << 5);
	MCHBAR32(0x5418) = reg32;
}

void haswell_early_initialization(int chipset_type)
{
	/* Setup all BARs required for early PCIe and raminit */
	haswell_setup_bars();

	/* Device Enable */
	pci_write_config32(PCI_DEV(0, 0, 0), DEVEN, DEVEN_HOST | DEVEN_IGD);

	haswell_setup_graphics();
}
