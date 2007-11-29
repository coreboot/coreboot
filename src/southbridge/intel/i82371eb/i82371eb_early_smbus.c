/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
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

/* TODO: Implement smbus_write_byte(), smbus_recv_byte(), smbus_send_byte(). */

#include <device/pci_ids.h>
#include "i82371eb.h"
#include "i82371eb_smbus.h"

#define SMBUS_IO_BASE 0x0f00

static void enable_smbus(void)
{
	device_t dev;
	uint8_t reg8;
	uint16_t reg16;

	dev = pci_locate_device(PCI_ID(PCI_VENDOR_ID_INTEL,
				PCI_DEVICE_ID_INTEL_82371AB_SMB_ACPI), 0);

	if (dev == PCI_DEV_INVALID) {
		die("SMBus controller not found\r\n");
	}
	print_spew("SMBus controller enabled\r\n");

	/* Set the SMBus I/O base. */
	pci_write_config32(dev, SMBBA, SMBUS_IO_BASE | 1);

	/* Enable the SMBus Controller Host Interface. */
	reg8 = pci_read_config8(dev, SMBHSTCFG);
	reg8 |= SMB_HST_EN;
	pci_write_config8(dev, SMBHSTCFG, reg8);

	/* Enable access to the SMBus I/O space. */
	reg16 = pci_read_config16(dev, PCI_COMMAND);
	reg16 |= IOSE;
	pci_write_config16(dev, PCI_COMMAND, reg16);

	/* Clear any lingering errors, so the transaction will run. */
	outb(inb(SMBUS_IO_BASE + SMBHST_STATUS), SMBUS_IO_BASE + SMBHST_STATUS);
}

static int smbus_read_byte(unsigned int device, unsigned int address)
{
	return do_smbus_read_byte(SMBUS_IO_BASE, device, address);
}
