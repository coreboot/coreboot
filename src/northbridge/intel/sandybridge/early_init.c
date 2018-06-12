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
#include <arch/acpi.h>
#include <device/pci_def.h>
#include <elog.h>
#include <cbmem.h>
#include <pc80/mc146818rtc.h>
#include <romstage_handoff.h>
#include "sandybridge.h"

static void sandybridge_setup_bars(void)
{
	/* Setting up Southbridge. In the northbridge code. */
	printk(BIOS_DEBUG, "Setting up static southbridge registers...");
	pci_write_config32(PCH_LPC_DEV, RCBA, (uintptr_t)DEFAULT_RCBA | 1);

	pci_write_config32(PCH_LPC_DEV, PMBASE, DEFAULT_PMBASE | 1);
	pci_write_config8(PCH_LPC_DEV, ACPI_CNTL, 0x80); /* Enable ACPI BAR */

	printk(BIOS_DEBUG, " done.\n");

	printk(BIOS_DEBUG, "Disabling Watchdog reboot...");
	RCBA32(GCS) = RCBA32(GCS) | (1 << 5);	/* No reset */
	outw((1 << 11), DEFAULT_PMBASE | 0x60 | 0x08);	/* halt timer */
	printk(BIOS_DEBUG, " done.\n");

	printk(BIOS_DEBUG, "Setting up static northbridge registers...");
	/* Set up all hardcoded northbridge BARs */
	pci_write_config32(PCI_DEV(0, 0x00, 0), EPBAR, DEFAULT_EPBAR | 1);
	pci_write_config32(PCI_DEV(0, 0x00, 0), EPBAR + 4, (0LL+DEFAULT_EPBAR) >> 32);
	pci_write_config32(PCI_DEV(0, 0x00, 0), MCHBAR, (uintptr_t)DEFAULT_MCHBAR | 1);
	pci_write_config32(PCI_DEV(0, 0x00, 0), MCHBAR + 4, (0LL+(uintptr_t)DEFAULT_MCHBAR) >> 32);
	pci_write_config32(PCI_DEV(0, 0x00, 0), DMIBAR, (uintptr_t)DEFAULT_DMIBAR | 1);
	pci_write_config32(PCI_DEV(0, 0x00, 0), DMIBAR + 4, (0LL+(uintptr_t)DEFAULT_DMIBAR) >> 32);

	/* Set C0000-FFFFF to access RAM on both reads and writes */
	pci_write_config8(PCI_DEV(0, 0x00, 0), PAM0, 0x30);
	pci_write_config8(PCI_DEV(0, 0x00, 0), PAM1, 0x33);
	pci_write_config8(PCI_DEV(0, 0x00, 0), PAM2, 0x33);
	pci_write_config8(PCI_DEV(0, 0x00, 0), PAM3, 0x33);
	pci_write_config8(PCI_DEV(0, 0x00, 0), PAM4, 0x33);
	pci_write_config8(PCI_DEV(0, 0x00, 0), PAM5, 0x33);
	pci_write_config8(PCI_DEV(0, 0x00, 0), PAM6, 0x33);

#if IS_ENABLED(CONFIG_ELOG_BOOT_COUNT)
	/* Increment Boot Counter for non-S3 resume */
	if ((inw(DEFAULT_PMBASE + PM1_STS) & WAK_STS) &&
	    ((inl(DEFAULT_PMBASE + PM1_CNT) >> 10) & 7) != SLP_TYP_S3)
		boot_count_increment();
#endif

	printk(BIOS_DEBUG, " done.\n");

#if IS_ENABLED(CONFIG_ELOG_BOOT_COUNT)
	/* Increment Boot Counter except when resuming from S3 */
	if ((inw(DEFAULT_PMBASE + PM1_STS) & WAK_STS) &&
	    ((inl(DEFAULT_PMBASE + PM1_CNT) >> 10) & 7) == SLP_TYP_S3)
		return;
	boot_count_increment();
#endif
}

static void sandybridge_setup_graphics(void)
{
	u32 reg32;
	u16 reg16;
	u8 reg8;
	u8 gfxsize;

	reg16 = pci_read_config16(PCI_DEV(0,2,0), PCI_DEVICE_ID);
	switch (reg16) {
	case 0x0102: /* GT1 Desktop */
	case 0x0106: /* GT1 Mobile */
	case 0x010a: /* GT1 Server */
	case 0x0112: /* GT2 Desktop */
	case 0x0116: /* GT2 Mobile */
	case 0x0122: /* GT2 Desktop >=1.3GHz */
	case 0x0126: /* GT2 Mobile >=1.3GHz */
	case 0x0152: /* IvyBridge */
	case 0x0156: /* IvyBridge */
	case 0x0162: /* IvyBridge */
	case 0x0166: /* IvyBridge */
	case 0x016a: /* IvyBridge */
		break;
	default:
		printk(BIOS_DEBUG, "Graphics not supported by this CPU/chipset.\n");
		return;
	}

	printk(BIOS_DEBUG, "Initializing Graphics...\n");

	if (get_option(&gfxsize, "gfx_uma_size") != CB_SUCCESS) {
		/* Setup IGD memory by setting GGC[7:3] = 1 for 32MB */
		gfxsize = 0;
	}
	reg16 = pci_read_config16(PCI_DEV(0,0,0), GGC);
	reg16 &= ~0x00f8;
	reg16 |= (gfxsize + 1) << 3;
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

static void start_peg_link_training(void)
{
	u32 tmp;
	u32 deven;

	/* PEG on IvyBridge+ needs a special startup sequence.
	 * As the MRC has its own initialization code skip it. */
	if (((pci_read_config16(PCI_DEV(0, 0, 0), PCI_DEVICE_ID) &
			BASE_REV_MASK) != BASE_REV_IVB) ||
		IS_ENABLED(CONFIG_HAVE_MRC))
		return;

	deven = pci_read_config32(PCI_DEV(0, 0, 0), DEVEN);

	if (deven & DEVEN_PEG10) {
		tmp = pci_read_config32(PCI_DEV(0, 1, 0), 0xC24) & ~(1 << 16);
		pci_write_config32(PCI_DEV(0, 1, 0), 0xC24, tmp | (1 << 5));
	}

	if (deven & DEVEN_PEG11) {
		tmp = pci_read_config32(PCI_DEV(0, 1, 1), 0xC24) & ~(1 << 16);
		pci_write_config32(PCI_DEV(0, 1, 1), 0xC24, tmp | (1 << 5));
	}

	if (deven & DEVEN_PEG12) {
		tmp = pci_read_config32(PCI_DEV(0, 1, 2), 0xC24) & ~(1 << 16);
		pci_write_config32(PCI_DEV(0, 1, 2), 0xC24, tmp | (1 << 5));
	}

	if (deven & DEVEN_PEG60) {
		tmp = pci_read_config32(PCI_DEV(0, 6, 0), 0xC24) & ~(1 << 16);
		pci_write_config32(PCI_DEV(0, 6, 0), 0xC24, tmp | (1 << 5));
	}
}

void sandybridge_early_initialization(int chipset_type)
{
	u32 capid0_a;
	u32 deven;
	u8 reg8;

	/* Device ID Override Enable should be done very early */
	capid0_a = pci_read_config32(PCI_DEV(0, 0, 0), 0xe4);
	if (capid0_a & (1 << 10)) {
		reg8 = pci_read_config8(PCI_DEV(0, 0, 0), 0xf3);
		reg8 &= ~7; /* Clear 2:0 */

		if (chipset_type == SANDYBRIDGE_MOBILE)
			reg8 |= 1; /* Set bit 0 */

		pci_write_config8(PCI_DEV(0, 0, 0), 0xf3, reg8);
	}

	/* Setup all BARs required for early PCIe and raminit */
	sandybridge_setup_bars();

	/* Setup IOMMU BARs */
	sandybridge_init_iommu();

	/* Device Enable, don't touch PEG bits */
	deven = pci_read_config32(PCI_DEV(0, 0, 0), DEVEN) | DEVEN_IGD;
	pci_write_config32(PCI_DEV(0, 0, 0), DEVEN, deven);

	sandybridge_setup_graphics();

	/* Write magic value to start PEG link training.
	 * This should be done in PCI device enumeration, but
	 * the PCIe specification requires to wait at least 100msec
	 * after reset for devices to come up.
	 * As we don't want to increase boot time, enable it early and
	 * assume the PEG is up as soon as PCI enumeration starts.
	 * TODO: use time stamps to ensure the timings are met */
	start_peg_link_training();
}

void northbridge_romstage_finalize(int s3resume)
{
	MCHBAR16(SSKPD) = 0xCAFE;

	romstage_handoff_init(s3resume);
}
