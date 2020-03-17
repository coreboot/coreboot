/*
 * This file is part of the coreboot project.
 *
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

#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <device/smbus_host.h>
#include "pch.h"

uintptr_t smbus_base(void)
{
	return SMBUS_IO_BASE;
}

int smbus_enable_iobar(uintptr_t base)
{
	/* Set the SMBus device statically. */
	pci_devfn_t dev = PCI_DEV(0x0, 0x1f, 0x3);

	/* Check to make sure we've got the right device. */
	if (pci_read_config16(dev, 0x0) != 0x8086)
		return -1;

	/* Set SMBus I/O base. */
	pci_write_config32(dev, SMB_BASE,
			   base | PCI_BASE_ADDRESS_SPACE_IO);

	/* Set SMBus enable. */
	pci_write_config8(dev, HOSTC, HST_EN);

	/* Set SMBus I/O space enable. */
	pci_write_config16(dev, PCI_COMMAND, PCI_COMMAND_IO);

	return 0;
}

int smbus_read_byte(unsigned int device, unsigned int address)
{
	return do_smbus_read_byte(SMBUS_IO_BASE, device, address);
}
