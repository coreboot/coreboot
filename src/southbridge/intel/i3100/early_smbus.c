/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Arastra, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#include "smbus.h"

#define SMBUS_IO_BASE 0x0f00

static void enable_smbus(void)
{
	device_t dev = PCI_DEV(0x0, 0x1f, 0x3);

	print_spew("SMBus controller enabled\n");
	pci_write_config32(dev, 0x20, SMBUS_IO_BASE | 1);
	pci_write_config8(dev, 0x40, 1);
	pci_write_config8(dev, 0x4, 1);
	/* SMBALERT_DIS */
        outb(4, SMBUS_IO_BASE + SMBSLVCMD);

	/* Disable interrupt generation */
	outb(0, SMBUS_IO_BASE + SMBHSTCTL);
}

static __attribute__((unused)) int smbus_read_byte(u32 device, u32 address)
{
	return do_smbus_read_byte(SMBUS_IO_BASE, device, address);
}

static __attribute__((unused)) int smbus_write_byte(unsigned device, u8 address, u8 data)
{
	return do_smbus_write_byte(SMBUS_IO_BASE, device, address, data);
}
