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
#if IS_ENABLED(CONFIG_VGA_ROM_RUN)
#include <x86emu/x86emu.h>
#endif
#include <arch/acpi.h>
#include <arch/io.h>
#include <arch/interrupt.h>
#include <boot/coreboot_tables.h>

/*
 * mainboard_enable is executed as first thing after enumerate_buses().
 * This is the earliest point to add customization.
 */
static void mainboard_enable(struct device *dev)
{

}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
