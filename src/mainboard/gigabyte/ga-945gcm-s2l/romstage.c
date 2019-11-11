/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 coresystems GmbH
 * Copyright (C) 2016 Arthur Heymans arthur@aheymans.xyz
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
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <cpu/x86/lapic.h>
#include <superio/ite/it8718f/it8718f.h>
#include <superio/ite/common/ite.h>
#include <console/console.h>
#include <arch/romstage.h>
#include <northbridge/intel/i945/i945.h>
#include <northbridge/intel/i945/raminit.h>
#include <southbridge/intel/i82801gx/i82801gx.h>
#include <southbridge/intel/common/pmclib.h>

#define SERIAL_DEV PNP_DEV(0x2e, IT8718F_SP1)
#define GPIO_DEV PNP_DEV(0x2e, IT8718F_GPIO)
#define EC_DEV PNP_DEV(0x2e, IT8718F_EC)
#define SUPERIO_DEV PNP_DEV(0x2e, 0)

static void setup_sio(void)
{
	/* Set default GPIOs on superio */
	ite_reg_write(GPIO_DEV, 0x25, 0x40);
	ite_reg_write(GPIO_DEV, 0x26, 0x3f);
	ite_reg_write(GPIO_DEV, 0x28, 0x41);
	ite_reg_write(GPIO_DEV, 0x29, 0x88);
	ite_reg_write(GPIO_DEV, 0x2c, 0x1c);
	ite_reg_write(GPIO_DEV, 0x62, 0x08);
	ite_kill_watchdog(GPIO_DEV);
	ite_reg_write(GPIO_DEV, 0xb1, 0x01);
	ite_reg_write(GPIO_DEV, 0xb8, 0x80);
	ite_reg_write(GPIO_DEV, 0xbb, 0x40);
	ite_reg_write(GPIO_DEV, 0xc0, 0x00);
	ite_reg_write(GPIO_DEV, 0xc3, 0x00);
	ite_reg_write(GPIO_DEV, 0xc8, 0x00);
	ite_reg_write(GPIO_DEV, 0xcb, 0x00);
	ite_reg_write(GPIO_DEV, 0xf6, 0x26);
	ite_reg_write(GPIO_DEV, 0xfc, 0x01);

	ite_reg_write(EC_DEV, 0x70, 0x00); // Don't use IRQ9
	ite_reg_write(EC_DEV, 0x30, 0xff); // Enable
}

static void rcba_config(void)
{
	/* Enable PCIe Root Port Clock Gate */
	RCBA32(CG) = 0x00000001;
}

void mainboard_romstage_entry(void)
{
	int s3resume = 0, boot_mode = 0;
	enable_lapic();

	i82801gx_lpc_setup();
	/* Enable SuperIO PM */
	setup_sio();
	ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);

	/* Disable SIO reboot */
	ite_reg_write(GPIO_DEV, 0xEF, 0x7E);

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
