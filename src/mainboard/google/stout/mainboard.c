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
#include <drivers/intel/gma/int15.h>
#include <pc80/mc146818rtc.h>
#include <arch/acpi.h>
#include <arch/io.h>
#include <arch/interrupt.h>
#include <boot/coreboot_tables.h>
#include "onboard.h"
#include "ec.h"
#include <southbridge/intel/bd82x6x/pch.h>
#include <smbios.h>
#include <device/pci.h>
#include <ec/quanta/it8518/ec.h>
#include <vendorcode/google/chromeos/chromeos.h>

void mainboard_suspend_resume(void)
{
	/* Stout EC needs to be put back in ACPI mode */
	ec_write_cmd(EC_CMD_NOTIFY_ACPI_ENTER);
}



static void mainboard_init(struct device *dev)
{
	struct device *ethernet_dev = NULL;

	/* Initialize the Embedded Controller */
	stout_ec_init();

	/*
	 * Battery life time - LAN PCIe should enter ASPM L1 to save
	 * power when LAN connection is idle.
	 * enable CLKREQ: LAN pci config space 0x81h=01
	 */
	ethernet_dev = dev_find_device(STOUT_NIC_VENDOR_ID,
				       STOUT_NIC_DEVICE_ID, dev);

	if (ethernet_dev != NULL)
		pci_write_config8(ethernet_dev, 0x81, 0x01);
}

// mainboard_enable is executed as first thing after
// enumerate_buses().

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_init;
	dev->ops->acpi_inject_dsdt_generator = chromeos_dsdt_generator;
	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_INT_LVDS, GMA_INT15_PANEL_FIT_DEFAULT, GMA_INT15_BOOT_DISPLAY_DEFAULT, 0);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
