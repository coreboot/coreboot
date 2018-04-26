/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
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
#include <device/device.h>
#include <delay.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include "bcm5785.h"

static void sata_init(struct device *dev)
{
	uint8_t byte;

	u8 *mmio;
	struct resource *res;
	u8 *mmio_base;
	int i;

	if (!(dev->path.pci.devfn & 7)) { // only set it in Func0
		byte = pci_read_config8(dev, 0x78);
		byte |= (1<<7);
		pci_write_config8(dev, 0x78, byte);

	        res = find_resource(dev, 0x24);
		mmio_base = res2mmio(res, 0, 3);

		write32(mmio_base + 0x10f0, 0x40000001);
		write32(mmio_base + 0x8c, 0x00ff2007);
		mdelay(10);
		write32(mmio_base + 0x8c, 0x78592009);
		mdelay(10);
		write32(mmio_base + 0x8c, 0x00082004);
		mdelay(10);
		write32(mmio_base + 0x8c, 0x00002004);
		mdelay(10);

		//init PHY

		printk(BIOS_DEBUG, "init PHY...\n");
		for (i=0; i<4; i++) {
			mmio = (u8 *)(uintptr_t)(res->base + 0x100 * i);
			byte = read8(mmio + 0x40);
			printk(BIOS_DEBUG, "port %d PHY status = %02x\n", i, byte);
			if (byte & 0x4) {// bit 2 is set
				byte = read8(mmio+0x48);
				write8(mmio + 0x48, byte | 1);
				write8(mmio + 0x48, byte & (~1));
				byte = read8(mmio + 0x40);
				printk(BIOS_DEBUG, "after reset port %d PHY status = %02x\n", i, byte);
			}
		}
	}
}

static void lpci_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	pci_write_config32(dev, 0x40,
		((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct pci_operations lops_pci = {
	.set_subsystem = lpci_set_subsystem,
};

static struct device_operations sata_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
//	.enable           = bcm5785_enable,
	.init             = sata_init,
	.scan_bus         = 0,
	.ops_pci          = &lops_pci,
};

static const struct pci_driver sata0_driver __pci_driver = {
	.ops    = &sata_ops,
	.vendor = PCI_VENDOR_ID_SERVERWORKS,
	.device = PCI_DEVICE_ID_SERVERWORKS_BCM5785_SATA,
};
