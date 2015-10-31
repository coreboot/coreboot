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
 */

#define __SIMPLE_DEVICE__

#include <arch/io.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <delay.h>

#ifdef __PRE_RAM__
unsigned pci_find_next_capability(pci_devfn_t dev, unsigned cap, unsigned last)
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

unsigned pci_find_capability(pci_devfn_t dev, unsigned cap)
{
	return pci_find_next_capability(dev, cap, 0);
}
#endif /* __PRE_RAM__ */


#if CONFIG_EARLY_PCI_BRIDGE

static void pci_bridge_reset_secondary(device_t p2p_bridge)
{
	u16 reg16;

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

static void pci_bridge_set_secondary(device_t p2p_bridge, u8 secondary)
{
	/* Disable config transaction forwarding. */
	pci_write_config8(p2p_bridge, PCI_SECONDARY_BUS, 0x00);
	pci_write_config8(p2p_bridge, PCI_SUBORDINATE_BUS, 0x00);
	/* Enable config transaction forwarding. */
	pci_write_config8(p2p_bridge, PCI_SECONDARY_BUS, secondary);
	pci_write_config8(p2p_bridge, PCI_SUBORDINATE_BUS, secondary);
}

static void pci_bridge_set_mmio(device_t p2p_bridge, u32 base, u32 size)
{
	u16 reg16;

	/* Disable MMIO window behind the bridge. */
	reg16 = pci_read_config16(p2p_bridge, PCI_COMMAND);
	reg16 &= ~PCI_COMMAND_MEMORY;
	pci_write_config16(p2p_bridge, PCI_COMMAND, reg16);
	pci_write_config32(p2p_bridge, PCI_MEMORY_BASE, 0x10);

	if (!size)
		return;

	/* Enable MMIO window behind the bridge. */
	pci_write_config32(p2p_bridge, PCI_MEMORY_BASE,
		((base + size - 1) & 0xfff00000) | ((base >> 16) & 0xfff0));

	reg16 = pci_read_config16(p2p_bridge, PCI_COMMAND);
	reg16 |= PCI_COMMAND_MEMORY;
	pci_write_config16(p2p_bridge, PCI_COMMAND, reg16);
}

void pci_early_bridge_init(void)
{
	int timeout, ret = -1;

	/* No PCI-to-PCI bridges are enabled yet, so the one we try to
	 * configure must have its primary on bus 0.
	 */
	pci_devfn_t p2p_bridge = PCI_DEV(0, CONFIG_EARLY_PCI_BRIDGE_DEVICE,
		CONFIG_EARLY_PCI_BRIDGE_FUNCTION);

	/* Secondary bus number is mostly irrelevant as we disable
	 * configuration transactions right after the probe.
	 */
	u8 secondary = 15;
	u8 dev = 0;
	u32 mmio_base = CONFIG_EARLY_PCI_MMIO_BASE;

	/* Enable configuration and MMIO over bridge. */
	pci_bridge_reset_secondary(p2p_bridge);
	pci_bridge_set_secondary(p2p_bridge, secondary);
	pci_bridge_set_mmio(p2p_bridge, mmio_base, 0x4000);

	for (timeout = 20000; timeout; timeout--) {
		u32 id = pci_read_config32(PCI_DEV(secondary, dev, 0), PCI_VENDOR_ID);
		if (id != 0 && id != 0xffffffff && id != 0xffff0001)
			break;
		udelay(10);
	}

	if (timeout != 0)
		ret = pci_early_device_probe(secondary, dev, mmio_base);

	/* Disable MMIO window if we found no suitable device. */
	if (ret)
		pci_bridge_set_mmio(p2p_bridge, 0, 0);

	/* Resource allocator will reconfigure bridges and secondary bus
	 * number may change. Thus early device cannot reliably use config
	 * transactions from here on, so we may as well disable them.
	 */
	pci_bridge_set_secondary(p2p_bridge, 0);
}
#endif /* CONFIG_EARLY_PCI_BRIDGE */
