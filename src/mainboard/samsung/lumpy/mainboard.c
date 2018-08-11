/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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
#include <drivers/intel/gma/int15.h>
#include <arch/acpi.h>
#include <arch/interrupt.h>
#include <boot/coreboot_tables.h>
#include <ec/smsc/mec1308/ec.h>
#include "ec.h"
#include "onboard.h"
#include <southbridge/intel/bd82x6x/pch.h>
#include <smbios.h>
#include <vendorcode/google/chromeos/chromeos.h>

void mainboard_suspend_resume(void)
{
	/* Enable EC ACPI mode for the OS before resume */
	send_ec_command(EC_SMI_DISABLE);
	send_ec_command(EC_ACPI_ENABLE);
}



static void mainboard_init(struct device *dev)
{
	/* Initialize the Embedded Controller */
	lumpy_ec_init();
}

static int lumpy_smbios_type41_irq(int *handle, unsigned long *current,
				   const char *name, u8 irq, u8 addr)
{
	struct smbios_type41 *t = (struct smbios_type41 *)*current;
	int len = sizeof(struct smbios_type41);

	memset(t, 0, sizeof(struct smbios_type41));
	t->type = SMBIOS_ONBOARD_DEVICES_EXTENDED_INFORMATION;
	t->handle = *handle;
	t->length = len - 2;
	t->reference_designation = smbios_add_string(t->eos, name);
	t->device_type = SMBIOS_DEVICE_TYPE_OTHER;
	t->device_status = 1;
	t->device_type_instance = irq;
	t->segment_group_number = 0;
	t->bus_number = addr;
	t->function_number = 0;
	t->device_number = 0;

	len = t->length + smbios_string_table_len(t->eos);
	*current += len;
	*handle += 1;
	return len;
}


static int lumpy_onboard_smbios_data(struct device *dev, int *handle,
				     unsigned long *current)
{
	int len = 0;

	len += lumpy_smbios_type41_irq(handle, current,
				       BOARD_LIGHTSENSOR_NAME,
				       BOARD_LIGHTSENSOR_IRQ,
				       BOARD_LIGHTSENSOR_I2C_ADDR);

	len += lumpy_smbios_type41_irq(handle, current,
				       BOARD_TRACKPAD_NAME,
				       BOARD_TRACKPAD_IRQ,
				       BOARD_TRACKPAD_I2C_ADDR);

	return len;
}

// mainboard_enable is executed as first thing after
// enumerate_buses().

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_init;
	dev->ops->get_smbios_data = lumpy_onboard_smbios_data;
	dev->ops->acpi_inject_dsdt_generator = chromeos_dsdt_generator;
	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_INT_LVDS, GMA_INT15_PANEL_FIT_DEFAULT, GMA_INT15_BOOT_DISPLAY_DEFAULT, 0);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
