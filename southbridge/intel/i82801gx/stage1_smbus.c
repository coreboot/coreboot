/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 coresystems GmbH
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <types.h>
#include <lib.h>
#include <console.h>
#include <device/pci.h>
#include <msr.h>
#include <legacy.h>
#include <device/pci.h>
#include <statictree.h>
#include <config.h>
#include <io.h>
#include "i82801gx.h"

void enable_smbus(void)
{
	u32 dev;

	/* Set the SMBus device statically. */
	dev = PCI_BDF(0x0, 0x1f, 0x3);

	/* Check to make sure we've got the right device. */
	if (pci_conf1_read_config16(dev, 0x2) != 0x27da) {
		die("SMBus controller not found!");
	}

	/* Set SMBus I/O base. */
	pci_conf1_write_config32(dev, SMB_BASE,
			   SMBUS_IO_BASE | PCI_BASE_ADDRESS_SPACE_IO);

	/* Set SMBus enable. */
	pci_conf1_write_config8(dev, HOSTC, HST_EN);

	/* Set SMBus I/O space enable. */
	pci_conf1_write_config16(dev, PCI_COMMAND, PCI_COMMAND_IO);

	/* Disable interrupt generation. */
	outb(0, SMBUS_IO_BASE + SMBHSTCTL);

	/* Clear any lingering errors, so transactions can run. */
	outb(inb(SMBUS_IO_BASE + SMBHSTSTAT), SMBUS_IO_BASE + SMBHSTSTAT);
	printk(BIOS_DEBUG, "SMBus controller enabled.\r\n");
}

/* some prototypes are hand-declared since the include files are still a little too stage-2 oriented */
int smbus_read_byte(u16 device, u16 address)
{
	int do_smbus_read_byte(unsigned device, unsigned address);
	return do_smbus_read_byte(device, address);
}

