/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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

#include <console/console.h>
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>

/* This is the AGP 3.0 "bridge" @ Bus 0 Device 1 Func 0 */

static void agp_bridge_init(struct device *dev)
{

	struct device *north_dev;
	u8 reg8;
	north_dev = dev_find_device(PCI_VENDOR_ID_VIA, 0x3324, 0);

	pci_write_config8(north_dev, 0xa0, 0x1);	// Enable CPU Direct Access Frame Buffer

	pci_write_config8(north_dev, 0xa2, 0x4a);

	reg8 = pci_read_config8(north_dev, 0xc0);
	reg8 |= 0x1;
	pci_write_config8(north_dev, 0xc0, reg8);

	/*
	 * Since Internal Graphic already set to AGP3.0 compatible in its Capability Pointer
	 * We must set RAGP8X=1 B0D0F0 Rx84[3]=1 from backdoor register B0D0F0 RxB5[1:0]=11b
	 */
	north_dev = dev_find_device(PCI_VENDOR_ID_VIA, 0x0324, 0);
	reg8 = pci_read_config8(north_dev, 0xb5);
	reg8 |= 0x3;
	pci_write_config8(north_dev, 0xb5, reg8);
	pci_write_config8(north_dev, 0x94, 0x20);
	pci_write_config8(north_dev, 0x13, 0xd0);

	pci_write_config16(dev, 0x4, 0x0007);

	pci_write_config8(dev, 0x19, 0x01);
	pci_write_config8(dev, 0x1a, 0x01);
	pci_write_config8(dev, 0x1c, 0xe0);
	pci_write_config8(dev, 0x1d, 0xe0);
	pci_write_config16(dev, 0x1e, 0xa220);

	pci_write_config16(dev, 0x20, 0xdd00);
	pci_write_config16(dev, 0x22, 0xdef0);
	pci_write_config16(dev, 0x24, 0xa000);
	pci_write_config16(dev, 0x26, 0xbff0);

	pci_write_config8(dev, 0x3e, 0x0c);
	pci_write_config8(dev, 0x40, 0x8b);
	pci_write_config8(dev, 0x41, 0x43);
	pci_write_config8(dev, 0x42, 0x62);
	pci_write_config8(dev, 0x43, 0x44);
	pci_write_config8(dev, 0x44, 0x34);
}

static struct device_operations agp_bridge_operations = {
	.read_resources = DEVICE_NOOP,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.init = agp_bridge_init,
	.scan_bus = pci_scan_bridge,
};

static const struct pci_driver agp_bridge_driver __pci_driver = {
	.ops = &agp_bridge_operations,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = 0xb198,
};
