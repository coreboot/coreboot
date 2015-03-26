/*
 * Copyright (C) 2014 Nico Huber <nico.h@gmx.de>
 *
 * Code borrowed from pci_early.c:
 *   Copyright (C) 2011 Google Inc
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
 * Foundation, Inc.
 */

#include <device/pci_ehci.h>

static unsigned pci_find_next_capability(pci_devfn_t dev, unsigned cap, unsigned last)
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
		unsigned this_cap;

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

static unsigned pci_find_capability(pci_devfn_t dev, unsigned cap)
{
	return pci_find_next_capability(dev, cap, 0);
}

extern void *ehci_bar;
int ehci_debug_hw_enable(unsigned int *base, unsigned int *dbg_offset)
{
	pci_devfn_t dbg_dev = pci_ehci_dbg_dev(CONFIG_USBDEBUG_HCD_INDEX);
	pci_ehci_dbg_enable(dbg_dev, CONFIG_EHCI_BAR);
	pci_devfn_t dev = dbg_dev;

	u8 pos = pci_find_capability(dev, PCI_CAP_ID_EHCI_DEBUG);
	if (!pos)
		return -1;

	u32 cap = pci_read_config32(dev, pos);

	/* FIXME: We should remove static EHCI_BAR_INDEX. */
	u8 dbg_bar = 0x10 + 4 * ((cap >> 29) - 1);
	if (dbg_bar != EHCI_BAR_INDEX)
		return -1;

	*base = (u32)ehci_bar;
	*dbg_offset = (cap>>16) & 0x1ffc;
	return 0;
}

void ehci_debug_select_port(unsigned int port)
{
	pci_devfn_t dbg_dev = pci_ehci_dbg_dev(CONFIG_USBDEBUG_HCD_INDEX);
	pci_ehci_dbg_set_port(dbg_dev, port);
}
