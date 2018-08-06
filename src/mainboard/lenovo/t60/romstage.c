/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 Sven Schnelle <svens@stackframe.org>
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

// __PRE_RAM__ means: use "unsigned" for device, not a struct.

#include <stdint.h>
#include <string.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <lib.h>
#include <arch/acpi.h>
#include <timestamp.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include <cpu/x86/bist.h>
#include <cpu/intel/romstage.h>
#include <halt.h>
#include <northbridge/intel/i945/i945.h>
#include <northbridge/intel/i945/raminit.h>
#include <southbridge/intel/i82801gx/i82801gx.h>
#include <southbridge/intel/common/gpio.h>
#include "dock.h"

static void ich7_enable_lpc(void)
{
	// Enable Serial IRQ
	pci_write_config8(PCI_DEV(0, 0x1f, 0), SERIRQ_CNTL, 0xd0);
	// decode range
	pci_write_config16(PCI_DEV(0, 0x1f, 0), LPC_IO_DEC, 0x0210);
	// decode range
	pci_write_config16(PCI_DEV(0, 0x1f, 0), LPC_EN, CNF1_LPC_EN | MC_LPC_EN
			| KBC_LPC_EN | GAMEH_LPC_EN | GAMEL_LPC_EN | FDD_LPC_EN
			| LPT_LPC_EN | COMA_LPC_EN);

	/* range 0x1600 - 0x167f */
	pci_write_config32(PCI_DEV(0, 0x1f, 0), GEN1_DEC, 0x007c1601);

	/* range 0x15e0 - 0x15ef */
	pci_write_config32(PCI_DEV(0, 0x1f, 0), GEN2_DEC, 0x000c15e1);

	/* range 0x1680 - 0x169f */
	pci_write_config32(PCI_DEV(0, 0x1f, 0), GEN3_DEC, 0x001c1681);
}

static void early_superio_config(void)
{
	int timeout = 100000;
	pnp_devfn_t dev = PNP_DEV(0x2e, 3);

	pnp_write_config(dev, 0x29, 0xa0);

	while(!(pnp_read_config(dev, 0x29) & 0x10) && timeout--)
		udelay(1000);

	/* Enable COM1 */
	pnp_set_logical_device(dev);
	pnp_set_iobase(dev, PNP_IDX_IO0, 0x3f8);
	pnp_set_enable(dev, 1);
}

static void rcba_config(void)
{
	/* Set up virtual channel 0 */
	RCBA32(0x0014) = 0x80000001;
	RCBA32(0x001c) = 0x03128010;

	/* Device 1f interrupt pin register */
	RCBA32(D31IP) = 0x00001230;
	RCBA32(D29IP) = 0x40004321;

	/* PCIe Interrupts */
	RCBA32(D28IP) = 0x00004321;
	/* HD Audio Interrupt */
	RCBA32(D27IP) = 0x00000002;

	/* dev irq route register */
	RCBA16(D31IR) = 0x1007;
	RCBA16(D30IR) = 0x0076;
	RCBA16(D29IR) = 0x3210;
	RCBA16(D28IR) = 0x7654;
	RCBA16(D27IR) = 0x0010;

	/* Enable IOAPIC */
	RCBA8(OIC) = 0x03;

	/* Disable unused devices */
	RCBA32(FD) = FD_PCIE6 | FD_PCIE5 | FD_INTLAN | FD_ACMOD | FD_ACAUD;
	RCBA32(FD) |= (1 << 0); // Required.

	/* Set up I/O Trap #0 for 0xfe00 (SMIC) */
	RCBA32(0x1e84) = 0x00020001;
	RCBA32(0x1e80) = 0x0000fe01;

	/* Set up I/O Trap #3 for 0x800-0x80c (Trap) */
	RCBA32(0x1e9c) = 0x000200f0;
	RCBA32(0x1e98) = 0x000c0801;
}

static void early_ich7_init(void)
{
	uint8_t reg8;
	uint32_t reg32;

	// program secondary mlt XXX byte?
	pci_write_config8(PCI_DEV(0, 0x1e, 0), 0x1b, 0x20);

	// reset rtc power status
	reg8 = pci_read_config8(PCI_DEV(0, 0x1f, 0), 0xa4);
	reg8 &= ~(1 << 2);
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0xa4, reg8);

	// usb transient disconnect
	reg8 = pci_read_config8(PCI_DEV(0, 0x1f, 0), 0xad);
	reg8 |= (3 << 0);
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0xad, reg8);

	reg32 = pci_read_config32(PCI_DEV(0, 0x1d, 7), 0xfc);
	reg32 |= (1 << 29) | (1 << 17);
	pci_write_config32(PCI_DEV(0, 0x1d, 7), 0xfc, reg32);

	reg32 = pci_read_config32(PCI_DEV(0, 0x1d, 7), 0xdc);
	reg32 |= (1 << 31) | (1 << 27);
	pci_write_config32(PCI_DEV(0, 0x1d, 7), 0xdc, reg32);

	RCBA32(0x0088) = 0x0011d000;
	RCBA16(0x01fc) = 0x060f;
	RCBA32(0x01f4) = 0x86000040;
	RCBA32(0x0214) = 0x10030549;
	RCBA32(0x0218) = 0x00020504;
	RCBA8(0x0220) = 0xc5;
	reg32 = RCBA32(GCS);
	reg32 |= (1 << 6);
	RCBA32(GCS) = reg32;
	reg32 = RCBA32(0x3430);
	reg32 &= ~(3 << 0);
	reg32 |= (1 << 0);
	RCBA32(0x3430) = reg32;
	RCBA32(FD) |= (1 << 0);
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

void mainboard_romstage_entry(unsigned long bist)
{
	int s3resume = 0;
	int dock_err;
	const u8 spd_addrmap[2 * DIMM_SOCKETS] = { 0x50, 0, 0x51, 0 };


	timestamp_init(get_initial_timestamp());
	timestamp_add_now(TS_START_ROMSTAGE);

	if (bist == 0)
		enable_lapic();

	ich7_enable_lpc();

	/* We want early GPIO setup, to be able to detect legacy I/O module */
	pci_write_config32(PCI_DEV(0, 0x1f, 0), GPIOBASE, DEFAULT_GPIOBASE | 1);
	/* Enable GPIOs */
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0x4c /* GC */ , 0x10);
	setup_pch_gpios(&mainboard_gpio_map);

	dock_err = dlpc_init();

	/* We prefer Legacy I/O module over docking */
	if (legacy_io_present()) {
		legacy_io_init();
		early_superio_config();
	} else if (!dock_err && dock_present()) {
		dock_connect();
		early_superio_config();
	}

	/* Setup the console */
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	if (MCHBAR16(SSKPD) == 0xCAFE) {
		printk(BIOS_DEBUG, "soft reset detected, rebooting properly\n");
		outb(0x6, 0xcf9);
		halt();
	}

	/* Perform some early chipset initialization required
	 * before RAM initialization can work
	 */
	i945_early_initialization();

	s3resume = southbridge_detect_s3_resume();

	/* Enable SPD ROMs and DDR-II DRAM */
	enable_smbus();

#if CONFIG_DEFAULT_CONSOLE_LOGLEVEL > 8
	dump_spd_registers();
#endif

	timestamp_add_now(TS_BEFORE_INITRAM);
	sdram_initialize(s3resume ? 2 : 0, spd_addrmap);
	timestamp_add_now(TS_AFTER_INITRAM);

	/* Perform some initialization that must run before stage2 */
	early_ich7_init();

	/* This should probably go away. Until now it is required
	 * and mainboard specific
	 */
	rcba_config();

	/* Chipset Errata! */
	fixup_i945_errata();

	/* Initialize the internal PCIe links before we go into stage2 */
	i945_late_initialization(s3resume);
}
