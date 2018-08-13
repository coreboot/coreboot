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
#include <arch/acpi.h>
#include <arch/io.h>
#include <boot/coreboot_tables.h>
#include <southbridge/intel/fsp_i89xx/pch.h>

#if IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)
void mainboard_suspend_resume(void)
{
	/* Call SMM finalize() handlers before resume */
	outb(0xcb, 0xb2);
}
#endif



// mainboard_enable is executed as first thing after
// enumerate_buses().

static void mainboard_enable(struct device *dev)
{
}



struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
