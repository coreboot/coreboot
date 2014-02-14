/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Google Inc
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

#include <arch/io.h>
#include <device/pci.h>
#include <device/pci_def.h>

unsigned pci_find_next_capability(device_t dev, unsigned cap, unsigned last)
{
	unsigned pos = 0;
	u16 status;
	unsigned reps = 48;

	status = pci_read_config16(dev, PCI_STATUS);
	if (!(status & PCI_STATUS_CAP_LIST))
		return 0;

	u8 hdr_type = pci_read_config8(dev, PCI_HEADER_TYPE);
	switch (hdr_type & 0x7f) {
	case PCI_HEADER_TYPE_NORMAL:
	case PCI_HEADER_TYPE_BRIDGE:
		pos = PCI_CAPABILITY_LIST;
		break;
	case PCI_HEADER_TYPE_CARDBUS:
		pos = PCI_CB_CAPABILITY_LIST;
		break;
	default:
		return 0;
	}

	pos = pci_read_config8(dev, pos);
	while (reps-- && (pos >= 0x40)) { /* Loop through the linked list. */
		int this_cap;

		pos &= ~3;
		this_cap = pci_read_config8(dev, pos + PCI_CAP_LIST_ID);
		if (this_cap == 0xff)
			break;

		if (!last && (this_cap == cap))
			return pos;

		if (last == pos)
			last = 0;

		pos = pci_read_config8(dev, pos + PCI_CAP_LIST_NEXT);
	}
	return 0;
}

unsigned pci_find_capability(device_t dev, unsigned cap)
{
	return pci_find_next_capability(dev, cap, 0);
}

void pci_bridge_reset_secondary(device_t p2p_bridge)
{
	u16 reg16;
	/* Disable all access through bridge. */
	reg16 = pci_read_config16(p2p_bridge, PCI_COMMAND);
	reg16 &= ~(PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY | PCI_COMMAND_IO);
	pci_write_config16(p2p_bridge, PCI_COMMAND, reg16);

	/* First we reset the secondary bus. */
	reg16 = pci_read_config16(p2p_bridge, PCI_BRIDGE_CONTROL);
	reg16 |= (1 << 6); /* SRESET */
	pci_write_config16(p2p_bridge, PCI_BRIDGE_CONTROL, reg16);

	/* Assume we don't have to wait here forever */

	/* Read back and clear reset bit. */
	reg16 = pci_read_config16(p2p_bridge, PCI_BRIDGE_CONTROL);
	reg16 &= ~(1 << 6); /* SRESET */
	pci_write_config16(p2p_bridge, PCI_BRIDGE_CONTROL, reg16);
}

void pci_bridge_set_secondary(device_t p2p_bridge, u8 secondary)
{
	/* Disable config transaction forwarding. */
	pci_write_config8(p2p_bridge, PCI_SUBORDINATE_BUS, 0x00);
	pci_write_config8(p2p_bridge, PCI_SECONDARY_BUS, 0x00);
	/* Enable config transaction forwarding. */
	pci_write_config8(p2p_bridge, PCI_SECONDARY_BUS, secondary);
	pci_write_config8(p2p_bridge, PCI_SUBORDINATE_BUS, secondary);
}

void pci_bridge_enable_mmio(device_t p2p_bridge, u32 base)
{
	u16 reg16;
	/* MMIO window behind the bridge. */
	pci_write_config32(p2p_bridge, PCI_MEMORY_BASE,
			((base & 0xffff0000) | ((base >> 16) & 0xff00)));

	/* Enable memory access through bridge */
	reg16 = pci_read_config16(p2p_bridge, PCI_COMMAND);
	reg16 |= PCI_COMMAND_MEMORY;
	pci_write_config16(p2p_bridge, PCI_COMMAND, reg16);
}
