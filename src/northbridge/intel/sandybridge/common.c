/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2012 The Chromium OS Authors
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

#include <types.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "sandybridge.h"

enum platform_type get_platform_type(void)
{
	const int id = get_platform_id();
	if (id != 1 && id != 4)
		printk(BIOS_WARNING, "WARN: Unknown platform id 0x%x\n", id);

	return (id == 4) ? PLATFORM_MOBILE : PLATFORM_DESKTOP_SERVER;
}
