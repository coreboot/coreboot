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
#include <device/pci.h>
#include <device/pnp.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <pc80/mc146818rtc.h>
#include <pc80/isa-dma.h>
#include <arch/io.h>
#include <device/smbus.h>
#include "bcm5785.h"
#include "smbus.h"

#define NMI_OFF 0

static void sb_init(device_t dev)
{
	uint8_t byte;
	uint8_t byte_old;
	int nmi_option;

	/* Set up NMI on errors */
	byte = inb(0x70); // RTC70
	byte_old = byte;
	nmi_option = NMI_OFF;
	get_option(&nmi_option, "nmi");
	if (nmi_option) {
		byte &= ~(1 << 7); /* set NMI */
	} else {
		byte |= (1 << 7); // Can not mask NMI from PCI-E and NMI_NOW
	}
	if (byte != byte_old) {
		outb(byte, 0x70);
	}


}

static void bcm5785_sb_read_resources(device_t dev)
{
	struct resource *res;

	/* Get the normal pci resources of this device */
	pci_dev_read_resources(dev);
	/* Get Resource for SMBUS */
	pci_get_resource(dev, 0x90);

	compact_resources(dev);

	/* Add an extra subtractive resource for both memory and I/O */
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
	res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;

	res = new_resource(dev, IOINDEX_SUBTRACTIVE(1, 0));
	res->flags = IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;

}

static int lsmbus_recv_byte(device_t dev)
{
	unsigned device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x90);

	return do_smbus_recv_byte(res->base, device);
}

static int lsmbus_send_byte(device_t dev, uint8_t val)
{
	unsigned device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x90);

	return do_smbus_send_byte(res->base, device, val);
}

static int lsmbus_read_byte(device_t dev, uint8_t address)
{
	unsigned device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x90);

	return do_smbus_read_byte(res->base, device, address);
}

static int lsmbus_write_byte(device_t dev, uint8_t address, uint8_t val)
{
	unsigned device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x90);

	return do_smbus_write_byte(res->base, device, address, val);
}

static struct smbus_bus_operations lops_smbus_bus = {
	.recv_byte  = lsmbus_recv_byte,
	.send_byte  = lsmbus_send_byte,
	.read_byte  = lsmbus_read_byte,
	.write_byte = lsmbus_write_byte,
};

static void lpci_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	pci_write_config32(dev, 0x2c,
		((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct pci_operations lops_pci = {
	.set_subsystem = lpci_set_subsystem,
};

static struct device_operations sb_ops = {
	.read_resources   = bcm5785_sb_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = sb_init,
	.scan_bus         = scan_smbus,
//        .enable           = bcm5785_enable,
	.ops_pci          = &lops_pci,
	.ops_smbus_bus    = &lops_smbus_bus,
};

static const struct pci_driver sb_driver __pci_driver = {
	.ops    = &sb_ops,
	.vendor = PCI_VENDOR_ID_SERVERWORKS,
	.device = PCI_DEVICE_ID_SERVERWORKS_BCM5785_SB_PCI_MAIN,
};
