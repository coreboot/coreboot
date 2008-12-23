/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 * Copyright (C) 2007 Ronald G. Minnich <rminnich@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#define _MAINOBJECT

#include <mainboard.h>
#include <config.h>
#include <types.h>
#include <lib.h>
#include <console.h>
#include <cpu.h>
#include <globalvars.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <msr.h>
#include <io.h>
#include <mc146818rtc.h>
#include <spd.h>
#include "../../../northbridge/intel/i945/ich7.h"
#include "../../../northbridge/intel/i945/i945.h"

#define RC0 ((1<<0)<<8)

#define DIMM0 0x50
#define DIMM1 0x51

/**
 * read a byte from spd.
 * @param device device to read from
 * @param address address in the spd ROM
 * @return the value of the byte at that address.
 */
u8 spd_read_byte(u16 device, u8 address)
{
	int do_smbus_read_byte(u16 device, u16 address);
	return do_smbus_read_byte(device, address);
}

static void early_ich7_init(void)
{
	u8 reg8;
	u32 reg32;

	// program secondary mlt XXX byte?
	pci_conf1_write_config8(PCI_BDF(0, 0x1e, 0), 0x1b, 0x20);

	// reset rtc power status
	reg8 = pci_conf1_read_config8(PCI_BDF(0, 0x1f, 0), 0xa4);
	reg8 &= ~(1 << 2);
	pci_conf1_write_config8(PCI_BDF(0, 0x1f, 0), 0xa4, reg8);

	// usb transient disconnect
	reg8 = pci_conf1_read_config8(PCI_BDF(0, 0x1f, 0), 0xad);
	reg8 |= (3 << 0);
	pci_conf1_write_config8(PCI_BDF(0, 0x1f, 0), 0xad, reg8);

	reg32 = pci_conf1_read_config32(PCI_BDF(0, 0x1d, 7), 0xfc);
	reg32 |= (1 << 29) | (1 << 17);
	pci_conf1_write_config32(PCI_BDF(0, 0x1d, 7), 0xfc, reg32);

	reg32 = pci_conf1_read_config32(PCI_BDF(0, 0x1d, 7), 0xdc);
	reg32 |= (1 << 31) | (1 << 27);
	pci_conf1_write_config32(PCI_BDF(0, 0x1d, 7), 0xdc, reg32);

	RCBA32(0x0088) = 0x0011d000;
	RCBA16(0x01fc) = 0x060f;
	RCBA32(0x01f4) = 0x86000040;
	RCBA32(0x0214) = 0x10030549;
	RCBA32(0x0218) = 0x00020504;
	RCBA8(0x0220) = 0xc5;
	reg32 = RCBA32(0x3410);
	reg32 |= (1 << 6);
	RCBA32(0x3410) = reg32;
	reg32 = RCBA32(0x3430);
	reg32 &= ~(3 << 0);
	reg32 |= (1 << 0);
	RCBA32(0x3430) = reg32;
	RCBA32(0x3418) |= (1 << 0);
	RCBA16(0x0200) = 0x2008;
	RCBA8(0x2027) = 0x0d;
	RCBA16(0x3e08) |= (1 << 7);
	RCBA16(0x3e48) |= (1 << 7);
	RCBA32(0x3e0e) |= (1 << 7);
	RCBA32(0x3e4e) |= (1 << 7);

	// next step only on ich7m b0 and later:
	reg32 = RCBA32(0x2034);
	reg32 &= ~(0x0f << 16);
	reg32 |= (5 << 16);
	RCBA32(0x2034) = reg32;
}
static void rcba_config(void)
{
	/* Set up virtual channel 0 */
	//RCBA32(0x0014) = 0x80000001;
	//RCBA32(0x001c) = 0x03128010;

	/* Device 1f interrupt pin register */
	RCBA32(0x3100) = 0x00042210;
	/* Device 1d interrupt pin register */
	RCBA32(0x310c) = 0x00214321;

	/* dev irq route register */
	RCBA16(0x3140) = 0x0132;
	RCBA16(0x3142) = 0x3241;
	RCBA16(0x3144) = 0x0237;
	RCBA16(0x3146) = 0x3210;
	RCBA16(0x3148) = 0x3210;

	/* Enable IOAPIC */
	RCBA8(0x31ff) = 0x03;

	/* Enable upper 128bytes of CMOS */
	RCBA32(0x3400) = (1 << 2);

	/* Disable unused devices */
	RCBA32(0x3418) = 0x000e0063;

	/* Enable PCIe Root Port Clock Gate */
	// RCBA32(0x341c) = 0x00000001;
}



/**
  * main for initram for the AMD DBM690T
 * @param init_detected Used to indicate that we have been started via init
 * @returns 0 on success
 * The purpose of this code is to not only get ram going, but get any other cpus/cores going.
 * The two activities are very tightly connected and not really seperable.
 *
  */
/*
 * init_detected is used to determine if we did a soft reset as required by a reprogramming of the
 * hypertransport links. If we did this kind of reset, bit 11 will be set in the MTRRdefType_MSR MSR.
 * That may seem crazy, but there are not lots of places to hide a bit when the CPU does a reset.
 * This value is picked up in assembly, or it should be.
 */
int main(void)
{
	int boot_mode = 0;
	void i945_early_initialization(void);
	void enable_smbus(void);
	int fixup_i945_errata(void);
	void i945_late_initialization(void);
	void sdram_initialize(int boot_mode);

	if (MCHBAR16(SSKPD) == 0xCAFE) {
		printk(BIOS_DEBUG, "soft reset detected.\n");
		boot_mode = 1;
	}

	/* Perform some early chipset initialization required
	 * before RAM initialization can work
	 */
	i945_early_initialization();

	/* Enable SPD ROMs and DDR-II DRAM */
	enable_smbus();
	
#if CONFIG_DEFAULT_CONSOLE_LOGLEVEL > 8
	dump_spd_registers();
#endif

	sdram_initialize(boot_mode);

	/* Perform some initialization that must run before stage2 */
	early_ich7_init();

	/* This should probably go away. Until now it is required 
	 * and mainboard specific 
	 */
	rcba_config();

	/* Chipset Errata! */
	fixup_i945_errata();

	/* Initialize the internal PCIe links before we go into stage2 */
	i945_late_initialization();

#if CONFIG_DEFAULT_CONSOLE_LOGLEVEL > 8
#if defined(DEBUG_RAM_SETUP)
	sdram_dump_mchbar_registers();
#endif

	{
		/* This will not work if TSEG is in place! */
		u32 tom = pci_conf1_read_config32(PCI_BDF(0,2,0), 0x5c);

		printk(BIOS_DEBUG, "TOM: 0x%08x\n", tom);
		ram_check(0x00000000, 0x000a0000);
		ram_check(0x00100000, tom);
	}
#endif
	MCHBAR16(SSKPD) = 0xCAFE;
	return 0;
}
