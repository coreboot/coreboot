/*
 * AMD 8111 "southbridge"
 * This file is part of the coreboot project.
 * Copyright 2004 Tyan Computer
 *  (Written by Yinghai Lu <yhlu@tyan.com>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */
#include <types.h>
#include <console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "ck804_smbus.h"

#define SMBUS_IO_BASE 0x1000

void enable_smbus(void)
{
	u32 dev;

	if (!pci_conf1_find_on_bus(0, PCI_VENDOR_ID_NVIDIA, 0x0052, &dev))
		die("SMBus controller not found\n");

	printk(BIOS_DEBUG,"SMBus controller enabled\n");

	/* Set SMBus I/O base. */
	pci_conf1_write_config32(dev, 0x20, SMBUS_IO_BASE | 1);

	/* Set SMBus I/O space enable. */
	pci_conf1_write_config16(dev, 0x4, 0x01);

	/* Clear any lingering errors, so the transaction will run. */
	outb(inb(SMBUS_IO_BASE + SMBHSTSTAT), SMBUS_IO_BASE + SMBHSTSTAT);
}

int smbus_read_byte(u16 dev, u16 address)
{
	return do_smbus_read_byte(SMBUS_IO_BASE, dev, address);
}

int smbus_write_byte(u16 dev, u16 address, u8 val)
{
	return do_smbus_write_byte(SMBUS_IO_BASE, dev, address, val);
}
