/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 Sven Schnelle <svens@stackframe.org>
 * Copyright (C) 2013 Vladimir Serbinenko <phcoder@gmail.com>
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

#include <console/console.h>
#include <device/device.h>
#include <arch/io.h>
#include <ec/acpi/ec.h>
#include <northbridge/intel/nehalem/nehalem.h>
#include <southbridge/intel/bd82x6x/pch.h>

#include "dock.h"
#include <drivers/intel/gma/int15.h>
#include <pc80/keyboard.h>
#include <cpu/x86/lapic.h>
#include <device/pci.h>
#include <drivers/lenovo/lenovo.h>
#include <arch/acpigen.h>

static acpi_cstate_t cst_entries[] = {
	{1, 1, 1000, {0x7f, 1, 2, {0}, 1, 0}},
	{2, 1, 500, {0x01, 8, 0, {0}, DEFAULT_PMBASE + LV2, 0}},
	{2, 17, 250, {0x01, 8, 0, {0}, DEFAULT_PMBASE + LV3, 0}},
};

int get_cst_entries(acpi_cstate_t ** entries)
{
	*entries = cst_entries;
	return ARRAY_SIZE(cst_entries);
}

static void mainboard_init(struct device *dev)
{
	printk(BIOS_SPEW, "starting SPI configuration\n");

	/* Configure SPI.  */
	RCBA32(0x3800) = 0x07ff0500;
	RCBA32(0x3804) = 0x3f046008;
	RCBA32(0x3808) = 0x0058efc0;
	RCBA32(0x384c) = 0x92000000;
	RCBA32(0x3850) = 0x00000a0b;
	RCBA32(0x3858) = 0x07ff0500;
	RCBA32(0x385c) = 0x04ff0003;
	RCBA32(0x3860) = 0x00020001;
	RCBA32(0x3864) = 0x00000fff;
	RCBA32(0x3874) = 0;
	RCBA32(0x3890) = 0xf8400000;
	RCBA32(0x3894) = 0x143b5006;
	RCBA32(0x3898) = 0x05200302;
	RCBA32(0x389c) = 0x0601209f;
	RCBA32(0x38b0) = 0x00000004;
	RCBA32(0x38b4) = 0x03040002;
	RCBA32(0x38c8) = 0x00002005;
	RCBA32(0x38c4) = 0x00802005;
	RCBA32(0x3804) = 0x3f04e008;

	printk(BIOS_SPEW, "SPI configured\n");
}

static void fill_ssdt(struct device *device)
{
	drivers_lenovo_serial_ports_ssdt_generate("\\_SB.PCI0.LPCB", 0);
}

static void mainboard_enable(struct device *dev)
{
	u16 pmbase;

	dev->ops->init = mainboard_init;
	dev->ops->acpi_fill_ssdt_generator = fill_ssdt;

	pmbase = pci_read_config32(dev_find_slot(0, PCI_DEVFN(0x1f, 0)),
				   PMBASE) & 0xff80;

	printk(BIOS_SPEW, " ... pmbase = 0x%04x\n", pmbase);

	outl(0, pmbase + SMI_EN);

	enable_lapic();
	pci_write_config32(dev_find_slot(0, PCI_DEVFN(0x1f, 0)), GPIO_BASE,
			   DEFAULT_GPIOBASE | 1);
	pci_write_config8(dev_find_slot(0, PCI_DEVFN(0x1f, 0)), GPIO_CNTL,
			  0x10);

	/* If we're resuming from suspend, blink suspend LED */
	if (acpi_is_wakeup_s3())
		ec_write(0x0c, 0xc7);

	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_INT_LVDS,
					GMA_INT15_PANEL_FIT_DEFAULT,
					GMA_INT15_BOOT_DISPLAY_LFP, 2);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
