/*
 * This file is part of the coreboot project.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdint.h>
#include <arch/io.h>
#include <console/console.h>
#include <device/pci_ids.h>
#include <device/pci_def.h>
#include "i82371eb.h"
#include "smbus.h"

void enable_smbus(void)
{
	device_t dev;
	u8 reg8;
	u16 reg16;

	/* Get the SMBus/PM device of the 82371AB/EB/MB. */
	dev = pci_locate_device(PCI_ID(PCI_VENDOR_ID_INTEL,
				PCI_DEVICE_ID_INTEL_82371AB_SMB_ACPI), 0);

	/* Set the SMBus I/O base. */
	pci_write_config32(dev, SMBBA, SMBUS_IO_BASE | 1);

	/* Enable the SMBus controller host interface. */
	reg8 = pci_read_config8(dev, SMBHSTCFG);
	reg8 |= SMB_HST_EN;
	pci_write_config8(dev, SMBHSTCFG, reg8);

	/* Enable access to the SMBus I/O space. */
	reg16 = pci_read_config16(dev, PCI_COMMAND);
	reg16 |= PCI_COMMAND_IO;
	pci_write_config16(dev, PCI_COMMAND, reg16);

	/* Clear any lingering errors, so the transaction will run. */
	outb(inb(SMBUS_IO_BASE + SMBHST_STATUS), SMBUS_IO_BASE + SMBHST_STATUS);
}

int smbus_read_byte(u8 device, u8 address)
{
	return do_smbus_read_byte(SMBUS_IO_BASE, device, address);
}
