/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 Sven Schnelle <svens@stackframe.org>
 * Copyright (C) 2013 Vladimir Serbinenko <phcoder@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdint.h>
#include <arch/io.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <cpu/x86/lapic.h>
#include <romstage_handoff.h>
#include <console/console.h>
#include <arch/romstage.h>
#include <ec/acpi/ec.h>
#include <timestamp.h>
#include <arch/acpi.h>

#include "dock.h"
#include <southbridge/intel/ibexpeak/pch.h>
#include <southbridge/intel/common/gpio.h>
#include <northbridge/intel/nehalem/nehalem.h>

#include <northbridge/intel/nehalem/raminit.h>
#include <southbridge/intel/ibexpeak/me.h>

static void pch_enable_lpc(void)
{
	/* EC Decode Range Port60/64, Port62/66 */
	/* Enable EC, PS/2 Keyboard/Mouse */
	pci_write_config16(PCH_LPC_DEV, LPC_EN,
			   CNF2_LPC_EN | CNF1_LPC_EN | MC_LPC_EN | KBC_LPC_EN |
			   COMA_LPC_EN | GAMEL_LPC_EN);

	pci_write_config32(PCH_LPC_DEV, LPC_GEN1_DEC, 0x7c1601);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN2_DEC, 0xc15e1);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN3_DEC, 0x1c1681);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN4_DEC, (0x68 & ~3) | 0x00040001);

	pci_write_config16(PCH_LPC_DEV, LPC_IO_DEC, 0x10);

	pci_write_config32(PCH_LPC_DEV, 0xd0, 0x0);
	pci_write_config32(PCH_LPC_DEV, 0xdc, 0x8);

	pci_write_config8(PCH_LPC_DEV, GEN_PMCON_3,
			  (pci_read_config8(PCH_LPC_DEV, GEN_PMCON_3) & ~2) | 1);

	pci_write_config32(PCH_LPC_DEV, ETR3,
			   pci_read_config32(PCH_LPC_DEV, ETR3) & ~ETR3_CF9GR);
}

const struct southbridge_usb_port mainboard_usb_ports[] = {
	/* Enabled, Current table lookup index, OC map */
	{ 1, IF1_557, 0 },
	{ 1, IF1_55F, 1 },
	{ 1, IF1_74B, 3 },
	{ 1, IF1_74B, 3 },
	{ 1, IF1_557, 3 },
	{ 1, IF1_14B, 3 },
	{ 1, IF1_74B, 3 },
	{ 1, IF1_74B, 3 },
	{ 1, IF1_74B, 4 },
	{ 1, IF1_74B, 5 },
	{ 1, IF1_55F, 7 },
	{ 1, IF1_55F, 7 },
	{ 1, IF1_557, 7 },
	{ 1, IF1_55F, 7 },
};

static void rcba_config(void)
{
	southbridge_configure_default_intmap();

	/* Must set BIT0 (hides performance counters PCI device).
	   coreboot enables the Rate Matching Hub which makes the UHCI PCI
	   devices disappear, so BIT5-12 and BIT28 can be set to hide those. */
	RCBA32(FD) = (1 << 28) | (0xff << 5) | 1;

	/* Set reserved bit to 1 */
	RCBA32(FD2) = 1;

	early_usb_init(mainboard_usb_ports);
}

static void set_fsb_frequency(void)
{
	u8 block[5];
	u16 fsbfreq = 62879;
	smbus_block_read(0x69, 0, 5, block);
	block[0] = fsbfreq;
	block[1] = fsbfreq >> 8;

	smbus_block_write(0x69, 0, 5, block);
}

void mainboard_romstage_entry(void)
{
	u32 reg32;
	int s3resume = 0;
	const u8 spd_addrmap[4] = { 0x50, 0, 0x51, 0 };
	enable_lapic();

	nehalem_early_initialization(NEHALEM_MOBILE);

	pch_enable_lpc();

	/* Enable USB Power. We need to do it early for usbdebug to work. */
	ec_set_bit(0x3b, 4);

	/* Enable GPIOs */
	pci_write_config32(PCH_LPC_DEV, GPIO_BASE, DEFAULT_GPIOBASE | 1);
	pci_write_config8(PCH_LPC_DEV, GPIO_CNTL, 0x10);

	setup_pch_gpios(&mainboard_gpio_map);

	pch_setup_cir(NEHALEM_MOBILE);


	/* This should probably go away. Until now it is required
	 * and mainboard specific
	 */
	rcba_config();

	console_init();

	/* Read PM1_CNT */
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

	outb((inb(DEFAULT_GPIOBASE | 0x3a) & ~0x2) | 0x20,
	     DEFAULT_GPIOBASE | 0x3a);
	outb(0x50, 0x15ec);
	outb(inb(0x15ee) & 0x70, 0x15ee);

	early_thermal_init();

	timestamp_add_now(TS_BEFORE_INITRAM);

	chipset_init(s3resume);

	set_fsb_frequency();

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
