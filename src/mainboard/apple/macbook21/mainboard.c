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

#include <console/console.h>
#include <device/device.h>
#include <arch/io.h>
#include <delay.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <arch/interrupt.h>
#include <northbridge/intel/i945/i945.h>
#include <pc80/mc146818rtc.h>
#include <arch/x86/include/arch/acpigen.h>
#include <drivers/intel/gma/int15.h>
#include <ec/acpi/ec.h>
#define PANEL INT15_5F35_CL_DISPLAY_DEFAULT

static acpi_cstate_t cst_entries[] = {
	{
		.ctype = 1,
		.latency = 1,
		.power = 1000,
		.resource = {
			.space_id = ACPI_ADDRESS_SPACE_FIXED,
			.bit_width = ACPI_FFIXEDHW_VENDOR_INTEL,
			.bit_offset = ACPI_FFIXEDHW_CLASS_MWAIT,
			{
				.resv = 0,
			},
			.addrl = 0,
			.addrh = 0,
		}
	},
	{
		.ctype = 2,
		.latency = 1,
		.power = 500,
		.resource = {
			.space_id = ACPI_ADDRESS_SPACE_FIXED,
			.bit_width = ACPI_FFIXEDHW_VENDOR_INTEL,
			.bit_offset = ACPI_FFIXEDHW_CLASS_MWAIT,
			{
				.resv = 0,
			},
			.addrl = 0x10,
			.addrh = 0,
		}
	},
};

int get_cst_entries(acpi_cstate_t **entries)
{
	*entries = cst_entries;
	return ARRAY_SIZE(cst_entries);
}

static void mainboard_init(struct device *dev)
{
	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_INT_LVDS, GMA_INT15_PANEL_FIT_DEFAULT, PANEL, 3);
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_init;
}

static void mainboard_final(void *chip_info)
{
	ec_set_bit(0x10, 2); /* switch off led */
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
	.final = mainboard_final,
};
