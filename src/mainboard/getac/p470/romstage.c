/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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
#include <string.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <lib.h>
#include <arch/acpi.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include <cpu/x86/bist.h>
#include <cpu/intel/romstage.h>
#include <halt.h>
#include <northbridge/intel/i945/i945.h>
#include <northbridge/intel/i945/raminit.h>
#include <southbridge/intel/i82801gx/i82801gx.h>
#include <timestamp.h>
#include "option_table.h"

static void setup_special_ich7_gpios(void)
{
	u32 gpios;

	printk(BIOS_SPEW, "\n  Initializing drive bay...\n");
	gpios = inl(DEFAULT_GPIOBASE + 0x38); // GPIO Level 2
	gpios |= (1 << 0); // GPIO33 = ODD
	gpios |= (1 << 1); // GPIO34 = IDE_RST#
	outl(gpios, DEFAULT_GPIOBASE + 0x38);	/* GP_LVL2 */

	gpios = inl(DEFAULT_GPIOBASE + 0x0c); // GPIO Level
	gpios &= ~(1 << 13);	// ??
	outl(gpios, DEFAULT_GPIOBASE + 0x0c);	/* GP_LVL */

	printk(BIOS_SPEW, "\n  Initializing Ethernet NIC...\n");
	gpios = inl(DEFAULT_GPIOBASE + 0x0c); // GPIO Level
	gpios &= ~(1 << 24);	// Enable LAN Power
	outl(gpios, DEFAULT_GPIOBASE + 0x0c);	/* GP_LVL */
}

static void ich7_enable_lpc(void)
{
	int lpt_en = 0;
	if (read_option(lpt, 0) != 0)
		lpt_en = LPT_LPC_EN;

	// Enable Serial IRQ
	pci_write_config8(PCI_DEV(0, 0x1f, 0), SERIRQ_CNTL, 0xd0);
	// decode range
	pci_write_config16(PCI_DEV(0, 0x1f, 0), LPC_IO_DEC, 0x0007);
	// decode range
	pci_write_config16(PCI_DEV(0, 0x1f, 0), LPC_EN, CNF2_LPC_EN | CNF1_LPC_EN
			| MC_LPC_EN | KBC_LPC_EN | GAMEH_LPC_EN | GAMEL_LPC_EN
			| FDD_LPC_EN| lpt_en | COMB_LPC_EN | COMA_LPC_EN);
	// Enable 0x02e0 - 0x2ff
	pci_write_config32(PCI_DEV(0, 0x1f, 0), GEN1_DEC, 0x001c02e1);
	// Enable 0x600 - 0x6ff
	pci_write_config32(PCI_DEV(0, 0x1f, 0), GEN2_DEC, 0x00fc0601);
	// Enable 0x68 - 0x6f
	pci_write_config32(PCI_DEV(0, 0x1f, 0), GEN3_DEC, 0x00040069);
}

/* This box has two superios, so enabling serial becomes slightly excessive.
 * We disable a lot of stuff to make sure that there are no conflicts between
 * the two. Also set up the GPIOs from the beginning. This is the "no schematic
 * but safe anyways" method.
 */
static void pnp_enter_ext_func_mode(pnp_devfn_t dev)
{
	unsigned int port = dev >> 8;
	outb(0x55, port);
}

static void pnp_exit_ext_func_mode(pnp_devfn_t dev)
{
	unsigned int port = dev >> 8;
	outb(0xaa, port);
}

static void pnp_write_register(pnp_devfn_t dev, int reg, int val)
{
	unsigned int port = dev >> 8;
	outb(reg, port);
	outb(val, port+1);
}

static void early_superio_config(void)
{
	pnp_devfn_t dev;

	dev = PNP_DEV(0x4e, 0x00);

	pnp_enter_ext_func_mode(dev);
	pnp_write_register(dev, 0x02, 0x0e); // UART power
	pnp_write_register(dev, 0x1b, (0x3e8 >> 2)); // UART3 base
	pnp_write_register(dev, 0x1c, (0x2e8 >> 2)); // UART4 base
	pnp_write_register(dev, 0x1d, (5 << 4) | 11); // UART3,4 IRQ
	pnp_write_register(dev, 0x1e, 1); // no 32khz clock
	pnp_write_register(dev, 0x24, (0x3f8 >> 2)); // UART1 base
	pnp_write_register(dev, 0x28, (4 << 4) | 0); // UART1,2 IRQ
	pnp_write_register(dev, 0x2c, 0); // DMA0 FIR
	pnp_write_register(dev, 0x30, (0x600 >> 4)); // Runtime Register Block Base

	pnp_write_register(dev, 0x31, 0xce); // GPIO1 DIR
	pnp_write_register(dev, 0x32, 0x00); // GPIO1 POL
	pnp_write_register(dev, 0x33, 0x0f); // GPIO2 DIR
	pnp_write_register(dev, 0x34, 0x00); // GPIO2 POL
	pnp_write_register(dev, 0x35, 0xa8); // GPIO3 DIR
	pnp_write_register(dev, 0x36, 0x00); // GPIO3 POL
	pnp_write_register(dev, 0x37, 0xa8); // GPIO4 DIR
	pnp_write_register(dev, 0x38, 0x00); // GPIO4 POL

	pnp_write_register(dev, 0x39, 0x00); // GPIO1 OUT
	pnp_write_register(dev, 0x40, 0x80); // GPIO2/MISC OUT
	pnp_write_register(dev, 0x41, 0x00); // GPIO5 OUT
	pnp_write_register(dev, 0x42, 0xa8); // GPIO5 DIR
	pnp_write_register(dev, 0x43, 0x00); // GPIO5 POL
	pnp_write_register(dev, 0x44, 0x00); // GPIO ALT1
	pnp_write_register(dev, 0x45, 0x50); // GPIO ALT2
	pnp_write_register(dev, 0x46, 0x00); // GPIO ALT3

	pnp_write_register(dev, 0x48, 0x55); // GPIO ALT5
	pnp_write_register(dev, 0x49, 0x55); // GPIO ALT6
	pnp_write_register(dev, 0x4a, 0x55); // GPIO ALT7
	pnp_write_register(dev, 0x4b, 0x55); // GPIO ALT8
	pnp_write_register(dev, 0x4c, 0x55); // GPIO ALT9
	pnp_write_register(dev, 0x4d, 0x55); // GPIO ALT10

	pnp_exit_ext_func_mode(dev);
}

static void rcba_config(void)
{
	/* Set up virtual channel 0 */
	//RCBA32(0x0014) = 0x80000001;
	//RCBA32(0x001c) = 0x03128010;

	/* Device 1f interrupt pin register */
	RCBA32(D31IP) = 0x00042220;
	/* Device 1d interrupt pin register */
	RCBA32(D28IP) = 0x00214321;

	/* dev irq route register */
	RCBA16(D31IR) = 0x0232;
	RCBA16(D30IR) = 0x3246;
	RCBA16(D29IR) = 0x0237;
	RCBA16(D28IR) = 0x3201;
	RCBA16(D27IR) = 0x3216;

	/* Enable IOAPIC */
	RCBA8(OIC) = 0x03;

	/* Disable unused devices */
	RCBA32(FD) = FD_PCIE6 | FD_PCIE5 | FD_INTLAN | FD_ACMOD | FD_ACAUD | FD_PATA;
	RCBA32(FD) |= (1 << 0); // Required.

	/* Enable PCIe Root Port Clock Gate */
	// RCBA32(0x341c) = 0x00000001;


	/* This should probably go into the ACPI enable trap */
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
	reg32 = RCBA32(0x3410);
	reg32 |= (1 << 6);
	RCBA32(0x3410) = reg32;
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

	timestamp_init(timestamp_get());
	timestamp_add_now(TS_START_ROMSTAGE);

	if (bist == 0)
		enable_lapic();

#if 0
	/* Force PCIRST# */
	pci_write_config16(PCI_DEV(0, 0x1e, 0), BCTRL, SBR);
	udelay(200 * 1000);
	pci_write_config16(PCI_DEV(0, 0x1e, 0), BCTRL, 0);
#endif

	ich7_enable_lpc();
	early_superio_config();

	/* Set up the console */
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

	setup_special_ich7_gpios();

	s3resume = southbridge_detect_s3_resume();

	/* Enable SPD ROMs and DDR-II DRAM */
	enable_smbus();

#if CONFIG_DEFAULT_CONSOLE_LOGLEVEL > 8
	dump_spd_registers();
#endif

	timestamp_add_now(TS_BEFORE_INITRAM);
	sdram_initialize(s3resume ? 2 : 0, NULL);
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
