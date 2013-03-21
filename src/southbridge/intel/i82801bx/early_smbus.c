/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 Tyan Computer
 * (Written by Yinghai Lu <yinghailu@gmail.com> for Tyan Computer)
 * Copyright (C) 2007 Corey Osgood <corey.osgood@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#include <arch/io.h>
#include <console/console.h>
#include <device/pci_ids.h>
#include <device/pci_def.h>
#include "i82801bx.h"
#include "smbus.h"

void enable_smbus(void)
{
	device_t dev;

	/* Set the SMBus device statically (D31:F3). */
	dev = PCI_DEV(0x0, 0x1f, 0x3);

	/* Set SMBus I/O base. */
	pci_write_config32(dev, SMB_BASE,
			   SMBUS_IO_BASE | PCI_BASE_ADDRESS_SPACE_IO);

	/* Set SMBus enable. */
	pci_write_config8(dev, HOSTC, HST_EN);

	/* Set SMBus I/O space enable. */
	pci_write_config16(dev, PCI_COMMAND, PCI_COMMAND_IO);

	/* Disable interrupt generation. */
	outb(0, SMBUS_IO_BASE + SMBHSTCTL);

	/* Clear any lingering errors, so transactions can run. */
	outb(inb(SMBUS_IO_BASE + SMBHSTSTAT), SMBUS_IO_BASE + SMBHSTSTAT);

	print_debug("SMBus controller enabled\n");
}

int smbus_read_byte(u8 device, u8 address)
{
	return do_smbus_read_byte(SMBUS_IO_BASE, device, address);
}
