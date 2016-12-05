/*
 * This file is part of the coreboot project.
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


#define __SIMPLE_DEVICE__

#include <arch/io.h>
#include <reset.h>
#include <device/pci_ids.h>


#define PCI_DEV_INVALID (0xffffffffU)
static pci_devfn_t pci_io_locate_device_on_bus(unsigned pci_id, unsigned bus)
{
	pci_devfn_t dev, last;
	dev = PCI_DEV(bus, 0, 0);
	last = PCI_DEV(bus, 31, 7);
	for (; dev <= last; dev += PCI_DEV(0,0,1)) {
		unsigned int id;
		id = pci_io_read_config32(dev, 0);
		if (id == pci_id) {
			return dev;
		}
	}
	return PCI_DEV_INVALID;
}

#include "../../../northbridge/amd/amdk8/reset_test.c"

void hard_reset(void)
{
	pci_devfn_t dev;
	unsigned bus;
	unsigned node = 0;
	unsigned link = get_sblk();

	/* Find the device.
	 * There can only be one 8111 on a hypertransport chain/bus.
	 */
	bus = node_link_to_bus(node, link);
	dev = pci_io_locate_device_on_bus(
		PCI_ID(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_8111_ISA),
		bus);

	/* Reset */
	set_bios_reset();
	pci_io_write_config8(dev, 0x47, 1);
}
