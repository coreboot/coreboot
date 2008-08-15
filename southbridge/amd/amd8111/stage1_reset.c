/*
 *
 * This file is part of the coreboot project.
 * Copyright (C) 2004 Stefan Reinauer <stepan@openbios.org>
 * Copyright (C) 2005-7 YingHai Lu
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

void hard_reset(void)
{
	u32 dev;
	unsigned int bus;
	unsigned int node = 0;
	unsigned int link = get_sblk();

	/* Find the device.
	 * There can only be one 8111 on a hypertransport chain/bus.
	 */
	bus = node_link_to_bus(node, link);
	pci_locate_device_on_bus(0, PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_8111_ISA, &dev);

	/* Reset */
	set_bios_reset();
	pci_write_config8(dev, 0x47, 1);
}
