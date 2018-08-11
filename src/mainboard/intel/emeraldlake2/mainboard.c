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
#include <arch/io.h>
#include <arch/interrupt.h>
#include <boot/coreboot_tables.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <vendorcode/google/chromeos/chromeos.h>

// mainboard_enable is executed as first thing after
// enumerate_buses().

static void mainboard_enable(struct device *dev)
{
	dev->ops->acpi_inject_dsdt_generator = chromeos_dsdt_generator;
	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_EDP, GMA_INT15_PANEL_FIT_CENTERING, GMA_INT15_BOOT_DISPLAY_DEFAULT, 0);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
