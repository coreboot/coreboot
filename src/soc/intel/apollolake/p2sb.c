/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
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

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <rules.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/p2sb.h>

#define P2SB_E0 0xe0
#define HIDE_BIT (1 << 0)

static void p2sb_set_hide_bit(int hide)
{
	struct device *dev;
	const uint16_t reg = P2SB_E0 + 1;
	const uint8_t mask = HIDE_BIT;
	uint8_t val;

	dev = PCH_DEV_P2SB;

	val = pci_read_config8(dev, reg);
	val &= ~mask;
	if (hide)
		val |= mask;
	pci_write_config8(dev, reg, val);
}

void p2sb_unhide(void)
{
	p2sb_set_hide_bit(0);
}

void p2sb_hide(void)
{
	p2sb_set_hide_bit(HIDE_BIT);
}

static void read_resources(struct device *dev)
{
	/*
	 * There's only one resource on the P2SB device. It's also already
	 * manually set to a fixed address in earlier boot stages.
	 */
	mmio_resource(dev, PCI_BASE_ADDRESS_0, P2SB_BAR / KiB, P2SB_SIZE / KiB);
}

static const struct device_operations device_ops = {
	.read_resources		= read_resources,
	.set_resources		= DEVICE_NOOP,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_INTEL_APL_P2SB,
	PCI_DEVICE_ID_INTEL_GLK_P2SB,
	0,
};

static const struct pci_driver pmc __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
