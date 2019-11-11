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

#include <stdint.h>
#include <cf9_reset.h>
#include <delay.h>
#include <device/pnp_ops.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <console/console.h>
#include <arch/romstage.h>
#include <northbridge/intel/i945/i945.h>
#include <northbridge/intel/i945/raminit.h>
#include <southbridge/intel/i82801gx/i82801gx.h>
#include <southbridge/intel/common/gpio.h>
#include <southbridge/intel/common/pmclib.h>
#include "dock.h"

/* Override the default lpc decode ranges */
static void mb_lpc_decode(void)
{
	// decode range
	pci_write_config16(PCI_DEV(0, 0x1f, 0), LPC_IO_DEC, 0x0210);
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
	RCBA32(V0CTL) = 0x80000001;

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

	/* Disable unused devices */
	RCBA32(FD) |= FD_INTLAN;

	/* Set up I/O Trap #0 for 0xfe00 (SMIC) */
	RCBA64(IOTR0) = 0x000200010000fe01ULL;

	/* Set up I/O Trap #3 for 0x800-0x80c (Trap) */
	RCBA64(IOTR3) = 0x000200f0000c0801ULL;
}

void mainboard_romstage_entry(void)
{
	int s3resume = 0;
	int dock_err;
	const u8 spd_addrmap[2 * DIMM_SOCKETS] = { 0x50, 0, 0x51, 0 };

	enable_lapic();

	/* Set up GPIO's early since it is needed for dock init */
	i82801gx_setup_bars();
	setup_pch_gpios(&mainboard_gpio_map);

	i82801gx_lpc_setup();
	mb_lpc_decode();

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

	if (MCHBAR16(SSKPD) == 0xCAFE) {
		system_reset();
	}

	/* Perform some early chipset initialization required
	 * before RAM initialization can work
	 */
	i82801gx_early_init();
	i945_early_initialization();

	s3resume = southbridge_detect_s3_resume();

	/* Enable SPD ROMs and DDR-II DRAM */
	enable_smbus();

	if (CONFIG(DEBUG_RAM_SETUP))
		dump_spd_registers();

	sdram_initialize(s3resume ? 2 : 0, spd_addrmap);

	/* This should probably go away. Until now it is required
	 * and mainboard specific
	 */
	rcba_config();

	/* Chipset Errata! */
	fixup_i945_errata();

	/* Initialize the internal PCIe links before we go into stage2 */
	i945_late_initialization(s3resume);
}
