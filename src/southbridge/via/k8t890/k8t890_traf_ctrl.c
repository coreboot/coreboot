/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License v2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <console/console.h>

#include "k8t890.h"

void mmconfig_set_resources(device_t dev)
{
	struct resource *resource;
	u8 reg;

	resource = find_resource(dev, K8T890_MMCONFIG_MBAR);
	if (resource) {
		report_resource_stored(dev, resource, "<mmconfig>");

		/* Remember this resource has been stored. */
		resource->flags |= IORESOURCE_STORED;
		pci_write_config8(dev, K8T890_MMCONFIG_MBAR,
				  (resource->base >> 28));
		reg = pci_read_config8(dev, 0x60);
		reg |= 0x3;
		/* Enable MMCONFIG decoding. */
		pci_write_config8(dev, 0x60, reg);
	}
	pci_dev_set_resources(dev);
}

static void apic_mmconfig_read_resources(device_t dev)
{
	struct resource *res;
	pci_dev_read_resources(dev);

	res = new_resource(dev, 0x40);
	/* NB APIC fixed to this address. */
	res->base = K8T890_APIC_BASE;
	res->size = 256;
	res->limit = res->base + res->size - 1;
	res->align = 8;
	res->gran = 8;
	res->flags = IORESOURCE_MEM | IORESOURCE_FIXED |
		     IORESOURCE_STORED | IORESOURCE_ASSIGNED;

	/* Add an MMCONFIG resource. */
	res = new_resource(dev, K8T890_MMCONFIG_MBAR);
	res->size = 256 * 1024 * 1024;
	res->align = log2(res->size);
	res->gran = log2(res->size);
	res->limit = 0xffffffff;	/* 4G */
	res->flags = IORESOURCE_MEM;
}

static void traf_ctrl_enable(struct device *dev)
{
	volatile u32 *apic;
	u32 data;

	/* Enable D3F1-D3F3, no device2 redirect, enable just one device behind
	 * bridge device 2 and device 3).
	 */
	pci_write_config8(dev, 0x60, 0x88);

	/* Will enable MMCONFIG later. */
	pci_write_config8(dev, 0x64, 0x23);
	/* No extended RCRB Base Address. */
	pci_write_config8(dev, 0x62, 0x00);

	/* Offset80 ->95 bit 4 in 1 in Award. */

	/* Enable APIC, to K8T890_APIC_BASE. */
	pci_write_config8(dev, 0x41, 0x00);
	pci_write_config8(dev, 0x40, 0x8c);
	/* BT_INTR enable, APIC Nonshare Mode Enable. */
	pci_write_config8(dev, 0x42, 0x5);

	apic = (u32 *)K8T890_APIC_BASE;

	/* Set APIC to FSB transported messages. */
	apic[0] = 3;
	data = apic[4];
	apic[4] = (data & 0xFFFFFE) | 1;

	/* Set APIC ID. */
	apic[0] = 0;
	data = apic[4];
	apic[4] = (data & 0xF0FFFF) | (K8T890_APIC_ID << 24);
}

static struct device_operations traf_ctrl_ops = {
	.read_resources = apic_mmconfig_read_resources,
	.set_resources = mmconfig_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.enable = traf_ctrl_enable,
	.ops_pci = 0,
};

static struct pci_driver northbridge_driver __pci_driver = {
	.ops = &traf_ctrl_ops,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_K8T890CE_5,
};
