/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
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
#include <cbmem.h>
#include <superio/smsc/lpc47m15x/lpc47m15x.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include <cpu/x86/bist.h>
#include <northbridge/intel/i945/i945.h>
#include <northbridge/intel/i945/raminit.h>
#include <southbridge/intel/i82801gx/i82801gx.h>

#define SERIAL_DEV PNP_DEV(0x2e, LPC47M15X_SP1)
#define PME_DEV PNP_DEV(0x2e, LPC47M15X_PME)

void setup_ich7_gpios(void)
{
	/* TODO: This is highly board specific and should be moved */
	printk(BIOS_DEBUG, " GPIOS...");
	/* General Registers */
	outl(0x3f3df7c1, DEFAULT_GPIOBASE + 0x00);	/* GPIO_USE_SEL */
	outl(0xc6fcbfc3, DEFAULT_GPIOBASE + 0x04);	/* GP_IO_SEL */
	outl(0xecfefdff, DEFAULT_GPIOBASE + 0x0c);	/* GP_LVL */
	/* Output Control Registers */
	outl(0x00040000, DEFAULT_GPIOBASE + 0x18);	/* GPO_BLINK */
	/* Input Control Registers */
	outl(0x0000a000, DEFAULT_GPIOBASE + 0x2c);	/* GPI_INV */
	outl(0x000000ff, DEFAULT_GPIOBASE + 0x30);	/* GPIO_USE_SEL2 */
	outl(0x000000bf, DEFAULT_GPIOBASE + 0x34);	/* GP_IO_SEL2 */
	outl(0x000300fd, DEFAULT_GPIOBASE + 0x38);	/* GP_LVL */
}

static void ich7_enable_lpc(void)
{
	// Enable Serial IRQ
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0x64, 0xd0);
	// Set COM1/COM2 decode range
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0x80, 0x0010);
	// Enable COM1
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0x82, 0x140d);
	// Enable SuperIO Power Management Events
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0x84, 0x007c0681);
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
	RCBA16(0x3142) = 0x0146;
	RCBA16(0x3144) = 0x0237;
	RCBA16(0x3146) = 0x3201;
	RCBA16(0x3148) = 0x0146;

	/* Enable IOAPIC */
	RCBA8(0x31ff) = 0x03;

	/* Disable unused devices */
	//RCBA32(0x3418) = FD_PCIE6|FD_PCIE5|FD_PCIE4|FD_ACMOD|FD_ACAUD|FD_PATA;
	// RCBA32(0x3418) |= (1 << 0); // Required.
	// FIXME look me up!
	RCBA32(0x3418) = 0x003204e1;

	/* Enable PCIe Root Port Clock Gate */
	// RCBA32(0x341c) = 0x00000001;
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

#include <cpu/intel/romstage.h>
void main(unsigned long bist)
{
	int s3resume = 0, boot_mode = 0;

	if (bist == 0)
		enable_lapic();

	ich7_enable_lpc();
	/* Enable SuperIO PM */
	lpc47m15x_enable_serial(PME_DEV, 0x680);
	lpc47m15x_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE); /* 0x3f8 */

	/* Set up the console */
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	if (MCHBAR16(SSKPD) == 0xCAFE) {
		printk(BIOS_DEBUG, "soft reset detected.\n");
		boot_mode = 1;
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

	sdram_initialize(s3resume ? 2 : boot_mode, NULL);

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
