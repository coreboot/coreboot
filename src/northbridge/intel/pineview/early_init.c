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

#include <stdint.h>
#include <stdlib.h>
#include <console/console.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <cbmem.h>
#include <halt.h>
#include <string.h>
#include <northbridge/intel/pineview/pineview.h>

#define LPC PCI_DEV(0, 0x1f, 0)
#define D0F0 PCI_DEV(0, 0, 0)

static void pineview_setup_bars(void)
{
	u8 reg8;
	u16 reg16;
	u32 reg32;

	/* Setting up Southbridge. In the northbridge code. */
	printk(BIOS_DEBUG, "Setting up static southbridge registers...");
	pci_write_config32(LPC, RCBA, (uintptr_t)DEFAULT_RCBA | 1);
	pci_write_config32(LPC, PMBASE, DEFAULT_PMBASE | 1);
	pci_write_config8(LPC, 0x44 /* ACPI_CNTL */ , 0x80); /* Enable ACPI */
	pci_write_config32(LPC, GPIOBASE, DEFAULT_GPIOBASE | 1);
	pci_write_config8(LPC, 0x4c /* GC */ , 0x10);	/* Enable GPIOs */
	pci_write_config32(LPC, 0x88, 0x007c0291);

	pci_write_config32(PCI_DEV(0, 0x1e, 0), 0x1b, 0x20);
	printk(BIOS_DEBUG, " done.\n");

	printk(BIOS_DEBUG, "Disabling Watchdog reboot...");
	RCBA32(GCS) = RCBA32(GCS) | (1 << 5);	/* No reset */
	outw((1 << 11), DEFAULT_PMBASE | 0x60 | 0x08);	/* halt timer */
	printk(BIOS_DEBUG, " done.\n");

	/* Enable upper 128bytes of CMOS */
	RCBA32(0x3400) = (1 << 2);

	printk(BIOS_DEBUG, "Setting up static northbridge registers...");
	pci_write_config8(D0F0, 0x8, 0x69);

	/* Set up all hardcoded northbridge BARs */
	pci_write_config32(D0F0, EPBAR, DEFAULT_EPBAR | 1);
	pci_write_config32(D0F0, MCHBAR, (uintptr_t)DEFAULT_MCHBAR | 1);
	pci_write_config32(D0F0, DMIBAR, (uintptr_t)DEFAULT_DMIBAR | 1);
	pci_write_config32(D0F0, PMIOBAR, (uintptr_t)0x400 | 1);


	reg32 = MCHBAR32(0x30);
	MCHBAR32(0x30) = 0x21800;
	DMIBAR32(0x2c) = 0x86000040;
	pci_write_config8(D0F0, DEVEN, 0x09);
	pci_write_config32(PCI_DEV(0, 0x1e, 0), 0x18, 0x00020200);
	pci_write_config32(PCI_DEV(0, 0x1e, 0), 0x18, 0x00000000);
	reg8 = pci_read_config8(D0F0, 0xe5);  // 0x10
	reg16 = pci_read_config16(PCI_DEV(0, 0x02, 0), 0x0); // 0x8086

	reg16 = pci_read_config16(D0F0, GGC);
	pci_write_config16(D0F0, GGC, 0x130);
	reg16 = pci_read_config16(D0F0, GGC);
	pci_write_config16(D0F0, GGC, 0x130);
	MCHBAR8(0xb08) = 0x20;
	reg8 = pci_read_config8(D0F0, 0xe6); // 0x11
	reg16 = MCHBAR16(0xc8c);
	MCHBAR16(0xc8c) = reg16 | 0x0200;
	reg8 = MCHBAR8(0xc8c);
	MCHBAR8(0xc8c) = reg8;
	MCHBAR8(0xc8c) = 0x12;
	pci_write_config8(PCI_DEV(0, 0x02, 0), 0x62, 0x02);
	pci_write_config16(PCI_DEV(0, 0x02, 0), 0xe8, 0x8000);
	MCHBAR32(0x3004) = 0x48000000;
	MCHBAR32(0x3008) = 0xfffffe00;
	MCHBAR32(0xb08) = 0x06028220;
	MCHBAR32(0xff4) = 0xc6db8b5f;
	MCHBAR16(0xff8) = 0x024f;

	// PLL Voltage controlled oscillator
	//MCHBAR8(0xc38) = 0x04;

	pci_write_config16(PCI_DEV(0, 0x02, 0), 0xcc, 0x014d);
	reg32 = MCHBAR32(0x40);
	MCHBAR32(0x40) = 0x0;
	reg32 = MCHBAR32(0x40);
	MCHBAR32(0x40) = 0x8;

	pci_write_config8(LPC, 0x8, 0x1d);
	pci_write_config8(LPC, 0x8, 0x0);
	RCBA32(0x3410) = 0x00020465;
	RCBA32(0x88) = 0x0011d000;
	RCBA32(0x1fc) = 0x60f;
	RCBA32(0x1f4) = 0x86000040;
	RCBA32(0x214) = 0x10030509;
	RCBA32(0x218) = 0x00020504;
	RCBA32(0x220) = 0xc5;
	RCBA32(0x3430) = 0x1;
	RCBA32(0x2027) = 0x38f6a70d;
	RCBA16(0x3e08) = 0x0080;
	RCBA16(0x3e48) = 0x0080;
	RCBA32(0x3e0e) = 0x00000080;
	RCBA32(0x3e4e) = 0x00000080;
	RCBA32(0x2034) = 0xb24577cc;
	RCBA32(0x1c) = 0x03128010;
	RCBA32(0x2010) = 0x400;
	RCBA32(0x3400) = 0x4;
	RCBA32(0x2080) = 0x18006007;
	RCBA32(0x20a0) = 0x18006007;
	RCBA32(0x20c0) = 0x18006007;
	RCBA32(0x20e0) = 0x18006007;

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

	/* Set C0000-FFFFF to access RAM on both reads and writes */
	pci_write_config8(D0F0, PAM0, 0x30);
	pci_write_config8(D0F0, PAM1, 0x33);
	pci_write_config8(D0F0, PAM2, 0x33);
	pci_write_config8(D0F0, PAM3, 0x33);
	pci_write_config8(D0F0, PAM4, 0x33);
	pci_write_config8(D0F0, PAM5, 0x33);
	pci_write_config8(D0F0, PAM6, 0x33);

	pci_write_config32(D0F0, SKPAD, SKPAD_NORMAL_BOOT_MAGIC);
	printk(BIOS_DEBUG, " done.\n");
}

void pineview_early_initialization(void)
{
	/* Print some chipset specific information */
	printk(BIOS_DEBUG, "Intel Pineview northbridge\n");

	/* Setup all BARs required for early PCIe and raminit */
	pineview_setup_bars();

	/* Change port80 to LPC */
	RCBA32(GCS) &= (~0x04);
	RCBA32(0x2010) |= (1 << 10);
}
