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

#include <device/pci_def.h>
#include <device/device.h>
#include <ec/lenovo/pmh7/pmh7.h>
#include <ec/lenovo/h8/h8.h>
#include <ec/acpi/ec.h>
#include <northbridge/intel/i945/i945.h>
#include <pc80/mc146818rtc.h>
#include <drivers/intel/gma/int15.h>
#include <arch/acpigen.h>

#define PANEL INT15_5F35_CL_DISPLAY_DEFAULT

static acpi_cstate_t cst_entries[] = {
	{ 1,  1, 1000, { 0x7f, 1, 2, { 0 }, 1, 0 } },
	{ 2,  1,  500, { 0x01, 8, 0, { 0 }, DEFAULT_PMBASE + LV2, 0 } },
	{ 2, 17,  250, { 0x01, 8, 0, { 0 }, DEFAULT_PMBASE + LV3, 0 } },
};

int get_cst_entries(acpi_cstate_t **entries)
{
	*entries = cst_entries;
	return ARRAY_SIZE(cst_entries);
}

static void mainboard_init(struct device *dev)
{
	struct southbridge_intel_i82801gx_config *config;
	struct device *idedev;

	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_INT_LVDS,
					GMA_INT15_PANEL_FIT_DEFAULT,
					PANEL, 3);

	/* If we're resuming from suspend, blink suspend LED */
	if (acpi_is_wakeup_s3())
		ec_write(0x0c, 0xc7);

	idedev = dev_find_slot(0, PCI_DEVFN(0x1f,1));

	if (!(inb(DEFAULT_GPIOBASE + 0x0c) & 0x40)) {
		/* legacy I/O connected */
		pmh7_ultrabay_power_enable(1);
		ec_write(0x0c, 0x84);
	} else if (idedev && idedev->chip_info &&
		   h8_ultrabay_device_present()) {
		config = idedev->chip_info;
		config->ide_enable_primary = 1;
		pmh7_ultrabay_power_enable(1);
		ec_write(0x0c, 0x84);
	} else {
		pmh7_ultrabay_power_enable(0);
		ec_write(0x0c, 0x04);
	}

	/* set dock status led */
	ec_write(0x0c, 0x08);
	ec_write(0x0c, inb(0x164c) & 8 ? 0x89 : 0x09);
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_init;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
