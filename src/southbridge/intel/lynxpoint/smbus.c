/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <device/device.h>
#include <device/path.h>
#include <device/smbus.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include "pch.h"
#include "smbus.h"

static void pch_smbus_init(device_t dev)
{
	struct resource *res;
	u16 reg16;

	/* Enable clock gating */
	reg16 = pci_read_config32(dev, 0x80);
	reg16 &= ~((1 << 8)|(1 << 10)|(1 << 12)|(1 << 14));
	pci_write_config32(dev, 0x80, reg16);

	/* Set Receive Slave Address */
	res = find_resource(dev, PCI_BASE_ADDRESS_4);
	if (res)
		outb(SMBUS_SLAVE_ADDR, res->base + SMB_RCV_SLVA);
}

static int lsmbus_read_byte(device_t dev, u8 address)
{
	u16 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);
	res = find_resource(pbus->dev, PCI_BASE_ADDRESS_4);

	return do_smbus_read_byte(res->base, device, address);
}

static int do_smbus_write_byte(unsigned smbus_base, unsigned device,
			       unsigned address, unsigned data)
{
	unsigned char global_status_register;

	if (smbus_wait_until_ready(smbus_base) < 0)
		return SMBUS_WAIT_UNTIL_READY_TIMEOUT;

	/* Setup transaction */
	/* Disable interrupts */
	outb(inb(smbus_base + SMBHSTCTL) & (~1), smbus_base + SMBHSTCTL);
	/* Set the device I'm talking too */
	outb(((device & 0x7f) << 1) & ~0x01, smbus_base + SMBXMITADD);
	/* Set the command/address... */
	outb(address & 0xff, smbus_base + SMBHSTCMD);
	/* Set up for a byte data read */
	outb((inb(smbus_base + SMBHSTCTL) & 0xe3) | (0x2 << 2),
	     (smbus_base + SMBHSTCTL));
	/* Clear any lingering errors, so the transaction will run */
	outb(inb(smbus_base + SMBHSTSTAT), smbus_base + SMBHSTSTAT);

	/* Clear the data byte... */
	outb(data, smbus_base + SMBHSTDAT0);

	/* Start the command */
	outb((inb(smbus_base + SMBHSTCTL) | 0x40),
	     smbus_base + SMBHSTCTL);

	/* Poll for transaction completion */
	if (smbus_wait_until_done(smbus_base) < 0) {
		printk(BIOS_ERR, "SMBUS transaction timeout\n");
		return SMBUS_WAIT_UNTIL_DONE_TIMEOUT;
	}

	global_status_register = inb(smbus_base + SMBHSTSTAT);

	/* Ignore the "In Use" status... */
	global_status_register &= ~(3 << 5);

	/* Read results of transaction */
	if (global_status_register != (1 << 1)) {
		printk(BIOS_ERR, "SMBUS transaction error\n");
		return SMBUS_ERROR;
	}

	return 0;
}

static int lsmbus_write_byte(device_t dev, u8 address, u8 data)
{
	u16 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);
	res = find_resource(pbus->dev, PCI_BASE_ADDRESS_4);
	return do_smbus_write_byte(res->base, device, address, data);
}

static struct smbus_bus_operations lops_smbus_bus = {
	.read_byte	= lsmbus_read_byte,
	.write_byte	= lsmbus_write_byte,
};

static void smbus_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	if (!vendor || !device) {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				pci_read_config32(dev, PCI_VENDOR_ID));
	} else {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				((device & 0xffff) << 16) | (vendor & 0xffff));
	}
}

static struct pci_operations smbus_pci_ops = {
	.set_subsystem    = smbus_set_subsystem,
};

static void smbus_read_resources(device_t dev)
{
	struct resource *res = new_resource(dev, PCI_BASE_ADDRESS_4);
	res->base = SMBUS_IO_BASE;
	res->size = 32;
	res->limit = res->base + res->size - 1;
	res->flags = IORESOURCE_IO | IORESOURCE_FIXED | IORESOURCE_RESERVE |
		     IORESOURCE_STORED | IORESOURCE_ASSIGNED;

	/* Also add MMIO resource */
	res = pci_get_resource(dev, PCI_BASE_ADDRESS_0);
}

static struct device_operations smbus_ops = {
	.read_resources		= smbus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.scan_bus		= scan_smbus,
	.init			= pch_smbus_init,
	.ops_smbus_bus		= &lops_smbus_bus,
	.ops_pci		= &smbus_pci_ops,
};

static const unsigned short pci_device_ids[] = { 0x1c22, 0x1e22, 0x9c22, 0 };

static const struct pci_driver pch_smbus __pci_driver = {
	.ops	 = &smbus_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
