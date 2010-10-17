/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
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

#include "ck804_smbus.h"

#define SMBUS_IO_BASE 0x1000

static void enable_smbus(void)
{
	device_t dev;
	dev = pci_locate_device(PCI_ID(0x10de, 0x0052), 0);
	if (dev == PCI_DEV_INVALID)
		die("SMBus controller not found\n");

	print_debug("SMBus controller enabled\n");

	/* Set SMBus I/O base. */
	pci_write_config32(dev, 0x20, SMBUS_IO_BASE | 1);

	/* Set SMBus I/O space enable. */
	pci_write_config16(dev, 0x4, 0x01);

	/* Clear any lingering errors, so the transaction will run. */
	outb(inb(SMBUS_IO_BASE + SMBHSTSTAT), SMBUS_IO_BASE + SMBHSTSTAT);
}

static int smbus_read_byte(unsigned device, unsigned address)
{
	return do_smbus_read_byte(SMBUS_IO_BASE, device, address);
}

static inline int smbus_write_byte(unsigned device, unsigned address,
			    unsigned char val)
{
	return do_smbus_write_byte(SMBUS_IO_BASE, device, address, val);
}
