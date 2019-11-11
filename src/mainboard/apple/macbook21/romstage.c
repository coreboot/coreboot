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
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <arch/romstage.h>
#include <cpu/x86/lapic.h>
#include <console/console.h>
#include <northbridge/intel/i945/i945.h>
#include <northbridge/intel/i945/raminit.h>
#include <southbridge/intel/i82801gx/i82801gx.h>
#include <southbridge/intel/common/pmclib.h>

static void rcba_config(void)
{
	/* V0CTL Virtual Channel 0 Resource Control */
	RCBA32(0x0014) = 0x80000001;

	/* Device 1f interrupt pin register */
	RCBA32(0x3100) = 0x00042210;
	RCBA32(0x3108) = 0x10004321;

	/* PCIe Interrupts */
	RCBA32(D28IP) = 0x00214321;
	/* HD Audio Interrupt */
	RCBA32(D27IP) = 0x00000001;

	/* dev irq route register */
	RCBA16(D31IR) = 0x0232;
	RCBA16(D30IR) = 0x3246;
	RCBA16(D29IR) = 0x0235;
	RCBA16(D28IR) = 0x3201;
	RCBA16(D27IR) = 0x3216;

	/* Disable unused devices */
	RCBA32(FD) |= FD_INTLAN;

	/* Set up I/O Trap #0 for 0xfe00 (SMIC) */

	/* Set up I/O Trap #3 for 0x800-0x80c (Trap) */
	RCBA32(0x1e9c) = 0x000200f0;
	RCBA32(0x1e98) = 0x000c0801;
}

void mainboard_romstage_entry(void)
{
	int s3resume = 0;
	const u8 spd_addrmap[2 * DIMM_SOCKETS] = { 0x50, 0x51, 0x52, 0x53 };

	enable_lapic();

	i82801gx_lpc_setup();

	/* Set up the console */
	console_init();

	if (MCHBAR16(SSKPD) == 0xCAFE) {
		printk(BIOS_DEBUG,
		       "Soft reset detected, rebooting properly.\n");
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
