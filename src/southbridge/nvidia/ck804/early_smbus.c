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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdint.h>
#include <arch/io.h>
#include <console/console.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>

#include "smbus.h"
#include "early_smbus.h"

#define SMBUS_BAR_BASE 0x20
#define SMBUS_IO_BASE 0x1000
#define SMBUS_IO_SIZE 0x0040

#define SMBUS_BAR(x) (SMBUS_BAR_BASE + 4 * (x))
#define SMBUS_BASE(x) (SMBUS_IO_BASE + SMBUS_IO_SIZE * (x))

void enable_smbus(void)
{
	device_t dev;

	dev = pci_locate_device(PCI_ID(PCI_VENDOR_ID_NVIDIA,
				PCI_DEVICE_ID_NVIDIA_CK804_SMB), 0);
	if (dev == PCI_DEV_INVALID)
		die("SMBus controller not found\n");

	/* Set SMBus I/O base. */
	pci_write_config32(dev, SMBUS_BAR(0), SMBUS_BASE(0) | 1);
	pci_write_config32(dev, SMBUS_BAR(1), SMBUS_BASE(1) | 1);

	/* Set SMBus I/O space enable. */
	pci_write_config16(dev, 0x4, 0x01);

	/* Clear any lingering errors, so the transaction will run. */
	outb(inb(SMBUS_BASE(0) + SMBHSTSTAT), SMBUS_BASE(0) + SMBHSTSTAT);
	outb(inb(SMBUS_BASE(1) + SMBHSTSTAT), SMBUS_BASE(1) + SMBHSTSTAT);

	print_debug("SMBus controller enabled\n");
}

int ck804_smbus_read_byte(unsigned bus, unsigned device, unsigned address)
{
	return do_smbus_read_byte(SMBUS_BASE(bus), device, address);
}

int ck804_smbus_write_byte(unsigned bus, unsigned device, unsigned address,
			   unsigned char val)
{
	return do_smbus_write_byte(SMBUS_BASE(bus), device, address, val);
}

int smbus_read_byte(unsigned device, unsigned address)
{
	return ck804_smbus_read_byte(0, device, address);
}

int smbus_write_byte(unsigned device, unsigned address, unsigned char val)
{
	return ck804_smbus_write_byte(0, device, address, val);
}
