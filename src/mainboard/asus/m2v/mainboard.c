/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Tobias Diedrich <ranma+coreboot@tdiedrich.de>
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

#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <console/console.h>
#include "southbridge/via/vt8237r/vt8237r.h"

u32 vt8237_ide_80pin_detect(struct device *dev)
{
	struct device *lpc_dev;
	u16 acpi_io_base;
	u32 gpio_in;
	u32 res;

	lpc_dev = dev_find_device(PCI_VENDOR_ID_VIA,
				PCI_DEVICE_ID_VIA_VT8237A_LPC, 0);
	if (!lpc_dev)
		return 0;

	acpi_io_base = pci_read_config16(lpc_dev, 0x88) & ~1;
	if (!acpi_io_base)
		return 0;

	gpio_in = inl(acpi_io_base + 0x48);
	/* bit 9 for primary port, clear if unconnected or 80-pin cable */
	res  = gpio_in & (1 << 9) ? 0 : VT8237R_IDE0_80PIN_CABLE;
	/* bit 4 for secondary port, clear if unconnected or 80-pin cable */
	res |= gpio_in & (1 << 4) ? 0 : VT8237R_IDE1_80PIN_CABLE;

	printk(BIOS_INFO, "Cable on %s PATA port: %d pin\n", "primary",
		res & VT8237R_IDE0_80PIN_CABLE ? 80 : 40);
	printk(BIOS_INFO, "Cable on %s PATA port: %d pin\n", "secondary",
		res & VT8237R_IDE1_80PIN_CABLE ? 80 : 40);

	return res;
}
