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
#include <device/pci_ids.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include <ec/acpi/ec.h>
#include <northbridge/intel/i945/i945.h>
#include <pc80/mc146818rtc.h>
#include "dock.h"
#include <drivers/intel/gma/int15.h>
#include <drivers/lenovo/lenovo.h>
#include <arch/acpigen.h>

#define PANEL INT15_5F35_CL_DISPLAY_DEFAULT

#define MWAIT_RES(state, sub_state)                         \
	{						    \
		.space_id = ACPI_ADDRESS_SPACE_FIXED,	    \
		.bit_width = ACPI_FFIXEDHW_VENDOR_INTEL,    \
		.bit_offset = ACPI_FFIXEDHW_CLASS_MWAIT,    \
		{					    \
			.resv = 0,			    \
		},					    \
		.addrl = (((state) << 4) | (sub_state)),    \
		.addrh = 0,				    \
			 }

static acpi_cstate_t cst_entries[] = {
	{
		.ctype = 1,
		.latency = 1,
		.power = 1000,
		.resource = MWAIT_RES(0, 0),
	},
	{
		.ctype = 2,
		.latency = 1,
		.power = 500,
		.resource = MWAIT_RES(1, 0),
	},
	{
		.ctype = 3,
		.latency = 17,
		.power = 250,
		.resource = MWAIT_RES(2, 0),
	},
};

int get_cst_entries(acpi_cstate_t **entries)
{
	*entries = cst_entries;
	return ARRAY_SIZE(cst_entries);
}

static void mainboard_init(struct device *dev)
{
	struct device *idedev, *sdhci_dev;

	ec_clr_bit(0x03, 2);

	if (inb(0x164c) & 0x08) {
		ec_set_bit(0x03, 2);
		ec_write(0x0c, 0x88);
	}

	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_INT_LVDS,
					GMA_INT15_PANEL_FIT_DEFAULT,
					PANEL, 3);

	/* If we're resuming from suspend, blink suspend LED */
	if (acpi_is_wakeup_s3())
		ec_write(0x0c, 0xc7);

	idedev = dev_find_slot(0, PCI_DEVFN(0x1f,1));
	if (idedev && idedev->chip_info && dock_ultrabay_device_present()) {
		struct southbridge_intel_i82801gx_config *config = idedev->chip_info;
		config->ide_enable_primary = 1;
		/* enable Ultrabay power */
		outb(inb(0x1628) | 0x01, 0x1628);
		ec_write(0x0c, 0x84);
	} else {
		/* disable Ultrabay power */
		outb(inb(0x1628) & ~0x01, 0x1628);
		ec_write(0x0c, 0x04);
	}

	/* Set SDHCI write protect polarity "SDWPPol" */
	sdhci_dev = dev_find_device(PCI_VENDOR_ID_RICOH, PCI_DEVICE_ID_RICOH_R5C822, 0);
	if (sdhci_dev) {
		if (pci_read_config8(sdhci_dev, 0xfa) != 0x20) {
			/* unlock */
			pci_write_config8(sdhci_dev, 0xf9, 0xfc);
			/* set SDWPPol, keep CLKRUNDis, SDPWRPol clear */
			pci_write_config8(sdhci_dev, 0xfa, 0x20);
			/* restore lock */
			pci_write_config8(sdhci_dev, 0xf9, 0x00);
		}
	}
}

static void fill_ssdt(struct device *device)
{
	drivers_lenovo_serial_ports_ssdt_generate("\\_SB.PCI0.LPCB", 1);
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_init;
	dev->ops->acpi_fill_ssdt_generator = fill_ssdt;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
