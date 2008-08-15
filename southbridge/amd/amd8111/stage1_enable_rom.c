/*
 *
 * This file is part of the coreboot project.
 * Copyright (C) 2003 Linux NetworX
 * Copyright (C) 2004 Ollie Lo
 * Copyright (C) 2006 YingHai Lu
 * Copyright (C) 2008 Ronald G. Minnich <rminnich@gmail.com>
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
#include <lib.h>
#include <console.h>
#include <device/pci.h>
#include <msr.h>
#include <legacy.h>
#include <device/pci_ids.h>
#include <statictree.h>
#include <config.h>

/**
 * Enable the 5 MB address space for the ROM
 */
static void amd8111_enable_rom(void)
{
	u8 byte;
	u32 dev;

	/* Enable 5MB rom access at 0xFFB00000 - 0xFFFFFFFF */
	/* Locate the amd8111 */
	pci_locate_device_on_bus(0, PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_8111_ISA, &dev);

	/* Set the 5MB enable bits */
	byte = pci_conf1_read_config8(dev, 0x43);
	byte |= 0xC0;
	pci_conf1_write_config8(dev, 0x43, byte);
}
