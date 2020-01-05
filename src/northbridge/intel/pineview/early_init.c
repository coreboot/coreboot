/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Damien Zammit <damien@zamaudio.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <device/pci.h>
#include <northbridge/intel/pineview/pineview.h>
#include <northbridge/intel/pineview/chip.h>
#include <option.h>
#include <types.h>

#define LPC PCI_DEV(0, 0x1f, 0)
#define D0F0 PCI_DEV(0, 0, 0)

#define PCI_GCFC	0xf0
#define MCH_GCFGC	0xc8c
#define  CRCLK_PINEVIEW	0x02
#define  CDCLK_PINEVIEW	0x10
#define MCH_HPLLVCO	0xc38

static void early_graphics_setup(void)
{
	u8 reg8;
	u16 reg16;
	u32 reg32;

	const struct device *d0f0 = pcidev_on_root(0, 0);
	const struct northbridge_intel_pineview_config *config = d0f0->chip_info;

	pci_write_config8(D0F0, DEVEN, BOARD_DEVEN);

	/* vram size from cmos option */
	if (get_option(&reg8, "gfx_uma_size") != CB_SUCCESS)
		reg8 = 0;	/* 0 for 8MB */
	/* make sure no invalid setting is used */
	if (reg8 > 6)
		reg8 = 0;
	/* Select 1M GTT */
	pci_write_config16(PCI_DEV(0, 0x00, 0), GGC, (1 << 8)
			   | ((reg8 + 3) << 4));

	printk(BIOS_SPEW, "Set GFX clocks...");
	reg16 = MCHBAR16(MCH_GCFGC);
	MCHBAR16(MCH_GCFGC) = reg16 | (1 << 9);
	reg16 &= ~0x7f;
	reg16 |= CDCLK_PINEVIEW | CRCLK_PINEVIEW;
	reg16 &= ~(1 << 9);
	MCHBAR16(MCH_GCFGC) = reg16;

	/* Graphics core */
	reg8 = MCHBAR8(MCH_HPLLVCO);
	reg8 &= 0x7;

	reg16 = pci_read_config16(PCI_DEV(0,2,0), 0xcc) & ~0x1ff;

	if (reg8 == 0x4) {
		/* 2666MHz */
		reg16 |= 0xad;
	} else if (reg8 == 0) {
		/* 3200MHz */
		reg16 |= 0xa0;
	} else if (reg8 == 1) {
		/* 4000MHz */
		reg16 |= 0xad;
	}

	pci_write_config16(PCI_DEV(0,2,0), 0xcc, reg16);

	pci_write_config8(PCI_DEV(0,2,0), 0x62,
		pci_read_config8(PCI_DEV(0,2,0), 0x62) & ~0x3);
	pci_write_config8(PCI_DEV(0,2,0), 0x62,
		pci_read_config8(PCI_DEV(0,2,0), 0x62) | 2);

	if (config->use_crt) {
		/* Enable VGA */
		MCHBAR32(0xb08) = MCHBAR32(0xb08) | (1 << 15);
	} else {
		/* Disable VGA */
		MCHBAR32(0xb08) = MCHBAR32(0xb08) & ~(1 << 15);
	}

	if (config->use_lvds) {
		/* Enable LVDS */
		reg32 = MCHBAR32(0x3004);
		reg32 &= ~0xf1000000;
		reg32 |= 0x90000000;
		MCHBAR32(0x3004) = reg32;
		MCHBAR32(0x3008) = MCHBAR32(0x3008) | (1 << 9);
	} else {
		/* Disable LVDS */
		MCHBAR32(0xb08) = MCHBAR32(0xb08) | (3 << 25);
	}

	MCHBAR32(0xff4) = 0x0c6db8b5f;
	MCHBAR16(0xff8) = 0x24f;

	MCHBAR32(0xb08) = MCHBAR32(0xb08) & 0xffffff00;
	MCHBAR32(0xb08) = MCHBAR32(0xb08) | (1 << 5);

	/* Legacy backlight control */
	pci_write_config8(PCI_DEV(0, 2, 0), 0xf4, 0x4c);
}

static void early_misc_setup(void)
{
	MCHBAR32(0x30);
	MCHBAR32(0x30) = 0x21800;
	DMIBAR32(0x2c) = 0x86000040;
	pci_write_config32(PCI_DEV(0, 0x1e, 0), 0x18, 0x00020200);
	pci_write_config32(PCI_DEV(0, 0x1e, 0), 0x18, 0x00000000);

	early_graphics_setup();

	MCHBAR32(0x40);
	MCHBAR32(0x40) = 0x0;
	MCHBAR32(0x40);
	MCHBAR32(0x40) = 0x8;

	pci_write_config8(LPC, 0x8, 0x1d);
	pci_write_config8(LPC, 0x8, 0x0);
	RCBA32(0x3410) = 0x00020465;

	pci_write_config32(PCI_DEV(0, 0x1d, 0), 0xca, 0x1);
	pci_write_config32(PCI_DEV(0, 0x1d, 1), 0xca, 0x1);
	pci_write_config32(PCI_DEV(0, 0x1d, 2), 0xca, 0x1);
	pci_write_config32(PCI_DEV(0, 0x1d, 3), 0xca, 0x1);

	RCBA32(0x3100) = 0x42210;
	RCBA32(0x3108) = 0x10004321;
	RCBA32(0x310c) = 0x00214321;
	RCBA32(0x3110) = 0x1;
	RCBA32(0x3140) = 0x01460132;
	RCBA32(0x3142) = 0x02370146;
	RCBA32(0x3144) = 0x32010237;
	RCBA32(0x3146) = 0x01463201;
	RCBA32(0x3148) = 0x146;
}

static void pineview_setup_bars(void)
{
	printk(BIOS_DEBUG, "Setting up static northbridge registers...");
	pci_write_config8(D0F0, 0x8, 0x69);

	/* Set up all hardcoded northbridge BARs */
	pci_write_config32(D0F0, EPBAR, DEFAULT_EPBAR | 1);
	pci_write_config32(D0F0, MCHBAR, (uintptr_t)DEFAULT_MCHBAR | 1);
	pci_write_config32(D0F0, DMIBAR, (uintptr_t)DEFAULT_DMIBAR | 1);
	pci_write_config32(D0F0, PMIOBAR, (uintptr_t)0x400 | 1);

	/* Set C0000-FFFFF to access RAM on both reads and writes */
	pci_write_config8(D0F0, PAM0, 0x30);
	pci_write_config8(D0F0, PAM1, 0x33);
	pci_write_config8(D0F0, PAM2, 0x33);
	pci_write_config8(D0F0, PAM3, 0x33);
	pci_write_config8(D0F0, PAM4, 0x33);
	pci_write_config8(D0F0, PAM5, 0x33);
	pci_write_config8(D0F0, PAM6, 0x33);

	printk(BIOS_DEBUG, " done.\n");
}

void pineview_early_initialization(void)
{
	/* Print some chipset specific information */
	printk(BIOS_DEBUG, "Intel Pineview northbridge\n");

	/* Setup all BARs required for early PCIe and raminit */
	pineview_setup_bars();

	/* Miscellaneous set up */
	early_misc_setup();

	/* Change port80 to LPC */
	RCBA32(GCS) &= (~0x04);
	RCBA32(0x2010) |= (1 << 10);
}
