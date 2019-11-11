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

#include <stdint.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <cpu/x86/lapic.h>
#include <superio/smsc/lpc47m15x/lpc47m15x.h>
#include <console/console.h>
#include <arch/romstage.h>
#include <northbridge/intel/i945/i945.h>
#include <northbridge/intel/i945/raminit.h>
#include <southbridge/intel/i82801gx/i82801gx.h>
#include <southbridge/intel/common/pmclib.h>

#define SERIAL_DEV PNP_DEV(0x2e, LPC47M15X_SP1)
#define PME_DEV PNP_DEV(0x2e, LPC47M15X_PME)

static void rcba_config(void)
{
	/* Set up virtual channel 0 */
	//RCBA32(0x0014) = 0x80000001;

	/* dev irq route register */
	RCBA16(D31IR) = 0x0132;
	RCBA16(D30IR) = 0x0146;
	RCBA16(D29IR) = 0x0237;
	RCBA16(D28IR) = 0x3201;
	RCBA16(D27IR) = 0x0146;

	/* Disable unused devices */
	RCBA32(FD) |= FD_INTLAN;

	/* Enable PCIe Root Port Clock Gate */
	// RCBA32(0x341c) = 0x00000001;
}

void mainboard_romstage_entry(void)
{
	int s3resume = 0, boot_mode = 0;

	enable_lapic();

	i82801gx_lpc_setup();
	/* Enable SuperIO PM */
	lpc47m15x_enable_serial(PME_DEV, 0x680);
	lpc47m15x_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE); /* 0x3f8 */

	/* Set up the console */
	console_init();

	if (MCHBAR16(SSKPD) == 0xCAFE) {
		printk(BIOS_DEBUG, "soft reset detected.\n");
		boot_mode = 1;
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

	sdram_initialize(s3resume ? 2 : boot_mode, NULL);

	/* This should probably go away. Until now it is required
	 * and mainboard specific
	 */
	rcba_config();

	/* Chipset Errata! */
	fixup_i945_errata();

	/* Initialize the internal PCIe links before we go into stage2 */
	i945_late_initialization(s3resume);
}
