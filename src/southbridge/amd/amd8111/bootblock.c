/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Linux Networx
 * (Written by Eric Biederman <ebiederman@lnxi.com> for Linux Networx)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdint.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <device/pci_type.h>

#define PCI_ID(VENDOR_ID, DEVICE_ID) \
	((((DEVICE_ID) & 0xFFFF) << 16) | ((VENDOR_ID) & 0xFFFF))

static pci_devfn_t pci_io_locate_device(unsigned int pci_id, pci_devfn_t dev)
{
	for (; dev <= PCI_DEV(255, 31, 7); dev += PCI_DEV(0, 0, 1)) {
		unsigned int id;
		id = pci_io_read_config32(dev, 0);
		if (id == pci_id)
			return dev;
	}
	return PCI_DEV_INVALID;
}

/* Enable 5MB ROM access at 0xFFB00000 - 0xFFFFFFFF. */
static void amd8111_enable_rom(void)
{
	u8 byte;
	pci_devfn_t dev;

	dev = pci_io_locate_device(PCI_ID(PCI_VENDOR_ID_AMD,
					  PCI_DEVICE_ID_AMD_8111_ISA), 0);

	/* Note: The 0xFFFF0000 - 0xFFFFFFFF range is always enabled. */

	/* Set the 5MB enable bits. */
	byte = pci_io_read_config8(dev, 0x43);
	byte |= (1 << 7); /* Enable 0xFFC00000-0xFFFFFFFF (4MB). */
	byte |= (1 << 6); /* Enable 0xFFB00000-0xFFBFFFFF (1MB). */
	pci_io_write_config8(dev, 0x43, byte);
}

static void bootblock_southbridge_init(void)
{
	amd8111_enable_rom();
}
