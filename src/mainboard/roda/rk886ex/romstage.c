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

/* Override the default lpc decode ranges */
static void mb_lpc_decode(void)
{
	int lpt_en = 0;
	if (read_option(lpt, 0) != 0)
		lpt_en = LPT_LPC_EN; /* enable LPT */

	pci_write_config16(PCI_DEV(0, 0x1f, 0), LPC_IO_DEC, 0x0007);

	pci_update_config16(PCI_DEV(0, 0x1f, 0), LPC_EN, ~LPT_LPC_EN, lpt_en);
}

/* This box has two superios, so enabling serial becomes slightly excessive.
 * We disable a lot of stuff to make sure that there are no conflicts between
 * the two. Also set up the GPIOs from the beginning. This is the "no schematic
 * but safe anyways" method.
 */
static inline void pnp_enter_ext_func_mode(pnp_devfn_t dev)
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

	dev = PNP_DEV(0x2e, 0x00);

	pnp_enter_ext_func_mode(dev);
	pnp_write_config(dev, 0x01, 0x94); /* Extended Parport modes */
	pnp_write_config(dev, 0x02, 0x88); /* UART power on */
	pnp_write_config(dev, 0x03, 0x72); /* Floppy */
	pnp_write_config(dev, 0x04, 0x01); /* EPP + SPP */
	pnp_write_config(dev, 0x14, 0x03); /* Floppy */
	pnp_write_config(dev, 0x20, (0x3f0 >> 2)); /* Floppy */
	pnp_write_config(dev, 0x23, (0x378 >> 2)); /* PP base */
	pnp_write_config(dev, 0x24, (0x3f8 >> 2)); /* UART1 base */
	pnp_write_config(dev, 0x25, (0x2f8 >> 2)); /* UART2 base */
	pnp_write_config(dev, 0x26, (2 << 4) | 0); /* FDC + PP DMA */
	pnp_write_config(dev, 0x27, (6 << 4) | 7); /* FDC + PP DMA */
	pnp_write_config(dev, 0x28, (4 << 4) | 3); /* UART1,2 IRQ */
	/* These are the SMI status registers in the SIO: */
	pnp_write_config(dev, 0x30, (0x600 >> 4)); /* Runtime Register Block Base */

	pnp_write_config(dev, 0x31, 0x00); /* GPIO1 DIR */
	pnp_write_config(dev, 0x32, 0x00); /* GPIO1 POL */
	pnp_write_config(dev, 0x33, 0x40); /* GPIO2 DIR */
	pnp_write_config(dev, 0x34, 0x00); /* GPIO2 POL */
	pnp_write_config(dev, 0x35, 0xff); /* GPIO3 DIR */
	pnp_write_config(dev, 0x36, 0x00); /* GPIO3 POL */
	pnp_write_config(dev, 0x37, 0xe0); /* GPIO4 DIR */
	pnp_write_config(dev, 0x38, 0x00); /* GPIO4 POL */
	pnp_write_config(dev, 0x39, 0x80); /* GPIO4 POL */

	pnp_exit_ext_func_mode(dev);
}

static void rcba_config(void)
{
	/* Set up virtual channel 0 */

	/* Device 1f interrupt pin register */
	RCBA32(D31IP) = 0x00042220;

	/* dev irq route register */
	RCBA16(D31IR) = 0x0232;
	RCBA16(D30IR) = 0x3246;
	RCBA16(D29IR) = 0x0237;
	RCBA16(D28IR) = 0x3201;
	RCBA16(D27IR) = 0x3216;

	/* Disable unused devices */
	RCBA32(FD) |= FD_INTLAN;

	/* This should probably go into the ACPI OS Init trap */

	/* Set up I/O Trap #0 for 0xfe00 (SMIC) */
	RCBA32(0x1e84) = 0x00020001;
	RCBA32(0x1e80) = 0x0000fe01;

	/* Set up I/O Trap #3 for 0x800-0x80c (Trap) */
	RCBA32(0x1e9c) = 0x000200f0;
	RCBA32(0x1e98) = 0x000c0801;
}

static void init_artec_dongle(void)
{
	/* Enable 4MB decoding */
	outb(0xf1, 0x88);
	outb(0xf4, 0x88);
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

	/* This has to happen after i945_early_initialization() */
	init_artec_dongle();

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
