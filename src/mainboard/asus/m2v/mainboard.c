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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <console/console.h>
#include "southbridge/via/vt8237r/vt8237r.h"
#include "chip.h"

u32 vt8237_ide_80pin_detect(struct device *dev)
{
	device_t lpc_dev;
	u16 acpi_io_base;
	u32 gpio_in;
	u32 res;

	lpc_dev = dev_find_device(PCI_VENDOR_ID_VIA,
				PCI_DEVICE_ID_VIA_VT8237A_LPC, 0);
	if (!lpc_dev)
		return 0;

	acpi_io_base = pci_read_config16(lpc_dev, 0x88);
	if (!acpi_io_base || (acpi_io_base & ~1) == 0)
		return 0;
	acpi_io_base &= ~1;

	gpio_in = inl(acpi_io_base + 0x48);
	/* bit 9 for primary port, clear if unconnected or 80-pin cable */
	res  = gpio_in & (1<<9) ? 0 : VT8237R_IDE0_80PIN_CABLE;
	/* bit 4 for secondary port, clear if unconnected or 80-pin cable */
	res |= gpio_in & (1<<4) ? 0 : VT8237R_IDE1_80PIN_CABLE;

	printk(BIOS_INFO, "Cable on %s PATA port: %d pin\n", "primary",
		gpio_in & (1<<9) ? 40 : 80);
	printk(BIOS_INFO, "Cable on %s PATA port: %d pin\n", "secondary",
		gpio_in & (1<<4) ? 40 : 80);

	return res;
}

struct chip_operations mainboard_ops = {
	CHIP_NAME("ASUS M2V")
};
