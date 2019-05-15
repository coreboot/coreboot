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

#include <device/device.h>
#include <smbios.h>
#if CONFIG(VGA_ROM_RUN)
#include <x86emu/x86emu.h>
#endif

#define BMC_KCS_BASE 0xca2
#define INTERFACE_IS_IO 0x1

#if CONFIG(GENERATE_SMBIOS_TABLES)
static int mainboard_smbios_data(struct device *dev, int *handle,
				 unsigned long *current)
{
	int len = 0;

	// add IPMI Device Information
	len += smbios_write_type38(
		current, handle,
		SMBIOS_BMC_INTERFACE_KCS,
		0x20,				// IPMI Version
		0x20,				// I2C address
		0xff,				// no NV storage
		BMC_KCS_BASE | INTERFACE_IS_IO, // IO port interface address
		0x40,
		0); // no IRQ

	return len;
}
#endif

/*
 * mainboard_enable is executed as first thing after enumerate_buses().
 * This is the earliest point to add customization.
 */
static void mainboard_enable(struct device *dev)
{
#if CONFIG(GENERATE_SMBIOS_TABLES)
	dev->ops->get_smbios_data = mainboard_smbios_data;
#endif

	/* Enable access to the BMC IPMI via KCS */
	struct device *lpc_sio_dev = dev_find_slot_pnp(BMC_KCS_BASE, 0);
	struct resource *res = new_resource(lpc_sio_dev, BMC_KCS_BASE);
	res->base = BMC_KCS_BASE;
	res->size = 1;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
