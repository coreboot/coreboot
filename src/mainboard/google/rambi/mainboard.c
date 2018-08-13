/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <types.h>
#include <string.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <console/console.h>
#if IS_ENABLED(CONFIG_VGA_ROM_RUN)
#include <x86emu/x86emu.h>
#endif
#include <arch/acpi.h>
#include <arch/io.h>
#include <arch/interrupt.h>
#include <boot/coreboot_tables.h>
#include <smbios.h>
#include "ec.h"
#include <variant/onboard.h>
#include <soc/gpio.h>
#include <bootstate.h>
#include <vendorcode/google/chromeos/chromeos.h>

void mainboard_suspend_resume(void)
{
}

#if IS_ENABLED(CONFIG_VGA_ROM_RUN)
static int int15_handler(void)
{
	int res = 1;

	printk(BIOS_DEBUG, "%s: AX=%04x BX=%04x CX=%04x DX=%04x\n",
	       __func__, X86_AX, X86_BX, X86_CX, X86_DX);

	switch (X86_AX) {
	case 0x5f34:
		/*
		 * Set Panel Fitting Hook:
		 *  bit 2 = Graphics Stretching
		 *  bit 1 = Text Stretching
		 *  bit 0 = Centering (do not set with bit1 or bit2)
		 *  0     = video bios default
		 */
		X86_AX = 0x005f;
		X86_CX = 0x0001;
		res = 1;
		break;
	case 0x5f35:
		/*
		 * Boot Display Device Hook:
		 *  bit 0 = CRT
		 *  bit 1 = TV
		 *  bit 2 = EFP (HDMI)
		 *  bit 3 = LFP (eDP)*
		 *  bit 4 = CRT2
		 *  bit 5 = TV2
		 *  bit 6 = EFP2
		 *  bit 7 = LFP2
		 */
		X86_AX = 0x005f;
		X86_CX = 0x0008;
		res = 1;
		break;
	case 0x5f51:
		/*
		 * Hook to select active LFP configuration:
		 *  00h = No LVDS, VBIOS does not enable LVDS
		 *  01h = Int-LVDS, LFP driven by integrated LVDS decoder
		 *  02h = SVDO-LVDS, LFP driven by SVDO decoder
		 *  03h = eDP, LFP Driven by Int-DisplayPort encoder
		 */
		X86_AX = 0x005f;
		X86_CX = 0x0003;
		res = 1;
		break;
	case 0x5f70:
		switch ((X86_CX >> 8) & 0xff) {
		case 0:
			/* Get Mux */
			X86_AX = 0x005f;
			X86_CX = 0x0000;
			res = 1;
			break;
		case 1:
			/* Set Mux */
			X86_AX = 0x005f;
			X86_CX = 0x0000;
			res = 1;
			break;
		case 2:
			/* Get SG/Non-SG mode */
			X86_AX = 0x005f;
			X86_CX = 0x0000;
			res = 1;
			break;
		default:
			/* Interrupt was not handled */
			printk(BIOS_DEBUG,
			       "Unknown INT15 5f70 function: 0x%02x\n",
				((X86_CX >> 8) & 0xff));
			break;
		}
		break;

	default:
		printk(BIOS_DEBUG, "Unknown INT15 function %04x!\n", X86_AX);
		break;
	}
	return res;
}
#endif

static void mainboard_init(struct device *dev)
{
	mainboard_ec_init();
#if IS_ENABLED(CONFIG_BOARD_GOOGLE_NINJA) || IS_ENABLED(CONFIG_BOARD_GOOGLE_SUMO)
	lan_init();
#endif
}

static int mainboard_smbios_data(struct device *dev, int *handle,
				 unsigned long *current)
{
	int len = 0;
#ifdef BOARD_TRACKPAD_NAME
	len += smbios_write_type41(
		current, handle,
		BOARD_TRACKPAD_NAME,            /* name */
		BOARD_TRACKPAD_IRQ,             /* instance */
		BOARD_TRACKPAD_I2C_BUS,         /* segment */
		BOARD_TRACKPAD_I2C_ADDR,        /* bus */
		0,                              /* device */
		0);                             /* function */
#endif
#ifdef BOARD_TOUCHSCREEN_NAME
	len += smbios_write_type41(
		current, handle,
		BOARD_TOUCHSCREEN_NAME,         /* name */
		BOARD_TOUCHSCREEN_IRQ,          /* instance */
		BOARD_TOUCHSCREEN_I2C_BUS,      /* segment */
		BOARD_TOUCHSCREEN_I2C_ADDR,     /* bus */
		0,                              /* device */
		0);                             /* function */
#endif
	return len;
}

// mainboard_enable is executed as first thing after
// enumerate_buses().

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_init;
	dev->ops->get_smbios_data = mainboard_smbios_data;
	dev->ops->acpi_inject_dsdt_generator = chromeos_dsdt_generator;
#if IS_ENABLED(CONFIG_VGA_ROM_RUN)
	/* Install custom int15 handler for VGA OPROM */
	mainboard_interrupt_handlers(0x15, &int15_handler);
#endif
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};

static void edp_vdden_cb(void *unused)
{
	ncore_select_func(SOC_DDI1_VDDEN_PAD, PAD_FUNC2);
}

BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_EXIT, edp_vdden_cb, NULL);
