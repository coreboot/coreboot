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
#include <arch/io.h>
#include <cf9_reset.h>
#include <delay.h>
#include <device/pnp_ops.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <cpu/x86/lapic.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include <arch/romstage.h>
#include <northbridge/intel/i945/i945.h>
#include <northbridge/intel/i945/raminit.h>
#include <southbridge/intel/i82801gx/i82801gx.h>
#include <southbridge/intel/common/pmclib.h>
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

/* Override the default lpc decode ranges */
static void mb_lpc_decode(void)
{
	int lpt_en = 0;
	if (read_option(lpt, 0) != 0)
		lpt_en = LPT_LPC_EN;

	// decode range
	pci_write_config16(PCI_DEV(0, 0x1f, 0), LPC_IO_DEC, 0x0007);
	// decode range
	pci_update_config32(PCI_DEV(0, 0x1f, 0), LPC_EN, ~LPT_LPC_EN, lpt_en);
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

static void early_superio_config(void)
{
	pnp_devfn_t dev;

	dev = PNP_DEV(0x4e, 0x00);

	pnp_enter_ext_func_mode(dev);
	pnp_write_config(dev, 0x02, 0x0e); // UART power
	pnp_write_config(dev, 0x1b, (0x3e8 >> 2)); // UART3 base
	pnp_write_config(dev, 0x1c, (0x2e8 >> 2)); // UART4 base
	pnp_write_config(dev, 0x1d, (5 << 4) | 11); // UART3,4 IRQ
	pnp_write_config(dev, 0x1e, 1); // no 32khz clock
	pnp_write_config(dev, 0x24, (0x3f8 >> 2)); // UART1 base
	pnp_write_config(dev, 0x28, (4 << 4) | 0); // UART1,2 IRQ
	pnp_write_config(dev, 0x2c, 0); // DMA0 FIR
	pnp_write_config(dev, 0x30, (0x600 >> 4)); // Runtime Register Block Base

	pnp_write_config(dev, 0x31, 0xce); // GPIO1 DIR
	pnp_write_config(dev, 0x32, 0x00); // GPIO1 POL
	pnp_write_config(dev, 0x33, 0x0f); // GPIO2 DIR
	pnp_write_config(dev, 0x34, 0x00); // GPIO2 POL
	pnp_write_config(dev, 0x35, 0xa8); // GPIO3 DIR
	pnp_write_config(dev, 0x36, 0x00); // GPIO3 POL
	pnp_write_config(dev, 0x37, 0xa8); // GPIO4 DIR
	pnp_write_config(dev, 0x38, 0x00); // GPIO4 POL

	pnp_write_config(dev, 0x39, 0x00); // GPIO1 OUT
	pnp_write_config(dev, 0x40, 0x80); // GPIO2/MISC OUT
	pnp_write_config(dev, 0x41, 0x00); // GPIO5 OUT
	pnp_write_config(dev, 0x42, 0xa8); // GPIO5 DIR
	pnp_write_config(dev, 0x43, 0x00); // GPIO5 POL
	pnp_write_config(dev, 0x44, 0x00); // GPIO ALT1
	pnp_write_config(dev, 0x45, 0x50); // GPIO ALT2
	pnp_write_config(dev, 0x46, 0x00); // GPIO ALT3

	pnp_write_config(dev, 0x48, 0x55); // GPIO ALT5
	pnp_write_config(dev, 0x49, 0x55); // GPIO ALT6
	pnp_write_config(dev, 0x4a, 0x55); // GPIO ALT7
	pnp_write_config(dev, 0x4b, 0x55); // GPIO ALT8
	pnp_write_config(dev, 0x4c, 0x55); // GPIO ALT9
	pnp_write_config(dev, 0x4d, 0x55); // GPIO ALT10

	pnp_exit_ext_func_mode(dev);
}

static void rcba_config(void)
{
	/* Set up virtual channel 0 */
	//RCBA32(0x0014) = 0x80000001;

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

	/* Disable unused devices */
	RCBA32(FD) |= FD_INTLAN;

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

void mainboard_romstage_entry(void)
{
	int s3resume = 0;

	enable_lapic();

	i82801gx_lpc_setup();
	mb_lpc_decode();
	early_superio_config();

	/* Set up the console */
	console_init();

	if (MCHBAR16(SSKPD) == 0xCAFE) {
		system_reset();
	}

	/* Perform some early chipset initialization required
	 * before RAM initialization can work
	 */
	i82801gx_early_init();
	i945_early_initialization();

	setup_special_ich7_gpios();

	s3resume = southbridge_detect_s3_resume();

	/* Enable SPD ROMs and DDR-II DRAM */
	enable_smbus();

	if (CONFIG(DEBUG_RAM_SETUP))
		dump_spd_registers();

	sdram_initialize(s3resume ? 2 : 0, NULL);

	/* This should probably go away. Until now it is required
	 * and mainboard specific
	 */
	rcba_config();

	/* Chipset Errata! */
	fixup_i945_errata();

	/* Initialize the internal PCIe links before we go into stage2 */
	i945_late_initialization(s3resume);
}
