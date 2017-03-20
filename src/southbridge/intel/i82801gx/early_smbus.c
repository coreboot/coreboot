/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
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

#include <arch/io.h>
#include <console/console.h>
#include <device/pci_ids.h>
#include <device/pci_def.h>
#include "i82801gx.h"
#include "smbus.h"

void enable_smbus(void)
{
	pci_devfn_t dev;

	/* Set the SMBus device statically. */
	dev = PCI_DEV(0x0, 0x1f, 0x3);

	/* Check to make sure we've got the right device. */
	if (pci_read_config16(dev, 0x2) != 0x27da)
		die("SMBus controller not found!");

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
	printk(BIOS_DEBUG, "SMBus controller enabled.\n");
}

int smbus_read_byte(unsigned int device, unsigned int address)
{
	return do_smbus_read_byte(SMBUS_IO_BASE, device, address);
}

int i2c_block_read(unsigned int device, unsigned int offset, u32 bytes, u8 *buf)
{
	u8 status;
	int bytes_read = 0;
	if (smbus_wait_until_ready(SMBUS_IO_BASE) < 0)
		return SMBUS_WAIT_UNTIL_READY_TIMEOUT;

	/* Setup transaction */
	/* Disable interrupts */
	outb(inb(SMBUS_IO_BASE + SMBHSTCTL) & (~1), SMBUS_IO_BASE + SMBHSTCTL);
	/* Set the device I'm talking to */
	outb((device & 0x7f) << 1, SMBUS_IO_BASE + SMBXMITADD);

	/* SPD offset */
	outb(offset, SMBUS_IO_BASE + SMBHSTDAT1);

	/* Set up for a i2c block data read */
	outb((inb(SMBUS_IO_BASE + SMBHSTCTL) & 0xc3) | (0x6 << 2),
		(SMBUS_IO_BASE + SMBHSTCTL));

	/* Clear any lingering errors, so the transaction will run */
	outb(inb(SMBUS_IO_BASE + SMBHSTSTAT), SMBUS_IO_BASE + SMBHSTSTAT);
	/* Start the command */
	outb((inb(SMBUS_IO_BASE + SMBHSTCTL) | 0x40),
	     SMBUS_IO_BASE + SMBHSTCTL);

	while (!(inb(SMBUS_IO_BASE + SMBHSTSTAT) & 1))
		;
	/* Poll for transaction completion */
	do {
		status = inb(SMBUS_IO_BASE + SMBHSTSTAT);
		if (status & ((1 << 4) | /* FAILED */
			      (1 << 3) | /* BUS ERR */
			      (1 << 2))) /* DEV ERR */
			return SMBUS_ERROR;

		if (status & 0x80) { /* Byte done */
			*buf = inb(SMBUS_IO_BASE + SMBBLKDAT);
			buf++;
			bytes_read++;
			if (--bytes == 1) {
				/* indicate that next byte is the last one */
				outb(inb(SMBUS_IO_BASE + SMBHSTCTL) | 0x20,
					 SMBUS_IO_BASE + SMBHSTCTL);
			}
			outb(status, SMBUS_IO_BASE + SMBHSTSTAT);
		}
	} while (status & 0x01);

	return bytes_read;
}
