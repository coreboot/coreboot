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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/path.h>
#include <device/smbus.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include "i82801gx.h"
#include "smbus.h"

static int lsmbus_read_byte(device_t dev, u8 address)
{
	u16 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);
	res = find_resource(pbus->dev, 0x20);

	return do_smbus_read_byte(res->base, device, address);
}

static int do_smbus_write_byte(unsigned smbus_base, unsigned device, unsigned address, unsigned data)
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
	if (smbus_wait_until_done(smbus_base) < 0)
		return SMBUS_WAIT_UNTIL_DONE_TIMEOUT;

	global_status_register = inb(smbus_base + SMBHSTSTAT);

	/* Ignore the "In Use" status... */
	global_status_register &= ~(3 << 5);

	/* Read results of transaction */
	if (global_status_register != (1 << 1))
		return SMBUS_ERROR;
	return 0;
}

static int lsmbus_write_byte(device_t dev, u8 address, u8 data)
{
	u16 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);
	res = find_resource(pbus->dev, 0x20);
	return do_smbus_write_byte(res->base, device, address, data);
}

static int do_smbus_block_write(unsigned smbus_base, unsigned device,
			      unsigned cmd, unsigned bytes, const u8 *buf)
{
	u8 status;

	if (smbus_wait_until_ready(smbus_base) < 0)
		return SMBUS_WAIT_UNTIL_READY_TIMEOUT;

	/* Setup transaction */
	/* Disable interrupts */
	outb(inb(smbus_base + SMBHSTCTL) & (~1), smbus_base + SMBHSTCTL);
	/* Set the device I'm talking too */
	outb(((device & 0x7f) << 1) & ~0x01, smbus_base + SMBXMITADD);
	/* Set the command/address... */
	outb(cmd & 0xff, smbus_base + SMBHSTCMD);
	/* Set up for a block data write */
	outb((inb(smbus_base + SMBHSTCTL) & 0xe3) | (0x5 << 2),
	     (smbus_base + SMBHSTCTL));
	/* Clear any lingering errors, so the transaction will run */
	outb(inb(smbus_base + SMBHSTSTAT), smbus_base + SMBHSTSTAT);

	/* set number of bytes to transfer */
	outb(bytes, smbus_base + SMBHSTDAT0);

	outb(*buf++, smbus_base + SMBBLKDAT);
	bytes--;

	/* Start the command */
	outb((inb(smbus_base + SMBHSTCTL) | 0x40),
	     smbus_base + SMBHSTCTL);

	while(!(inb(smbus_base + SMBHSTSTAT) & 1));
	/* Poll for transaction completion */
	do {
		status = inb(smbus_base + SMBHSTSTAT);
		if (status & ((1 << 4) | /* FAILED */
			      (1 << 3) | /* BUS ERR */
			      (1 << 2))) /* DEV ERR */
			return SMBUS_ERROR;

		if (status & 0x80) { /* Byte done */
			outb(*buf++, smbus_base + SMBBLKDAT);
			outb(status, smbus_base + SMBHSTSTAT);
		}
	} while(status & 0x01);

	return 0;
}



static int lsmbus_block_write(device_t dev, u8 cmd, u8 bytes, const u8 *buf)
{
	u16 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);
	res = find_resource(pbus->dev, 0x20);
	return do_smbus_block_write(res->base, device, cmd, bytes, buf);
}

static int do_smbus_block_read(unsigned smbus_base, unsigned device,
			      unsigned cmd, unsigned bytes, u8 *buf)
{
	u8 status;
	int bytes_read = 0;
	if (smbus_wait_until_ready(smbus_base) < 0)
		return SMBUS_WAIT_UNTIL_READY_TIMEOUT;

	/* Setup transaction */
	/* Disable interrupts */
	outb(inb(smbus_base + SMBHSTCTL) & (~1), smbus_base + SMBHSTCTL);
	/* Set the device I'm talking too */
	outb(((device & 0x7f) << 1) | 1, smbus_base + SMBXMITADD);
	/* Set the command/address... */
	outb(cmd & 0xff, smbus_base + SMBHSTCMD);
	/* Set up for a block data read */
	outb((inb(smbus_base + SMBHSTCTL) & 0xe3) | (0x5 << 2),
	     (smbus_base + SMBHSTCTL));
	/* Clear any lingering errors, so the transaction will run */
	outb(inb(smbus_base + SMBHSTSTAT), smbus_base + SMBHSTSTAT);

	/* Start the command */
	outb((inb(smbus_base + SMBHSTCTL) | 0x40),
	     smbus_base + SMBHSTCTL);

	while(!(inb(smbus_base + SMBHSTSTAT) & 1));
	/* Poll for transaction completion */
	do {
		status = inb(smbus_base + SMBHSTSTAT);
		if (status & ((1 << 4) | /* FAILED */
			      (1 << 3) | /* BUS ERR */
			      (1 << 2))) /* DEV ERR */
			return SMBUS_ERROR;

		if (status & 0x80) { /* Byte done */
			*buf = inb(smbus_base + SMBBLKDAT);
			buf++;
			bytes_read++;
			outb(status, smbus_base + SMBHSTSTAT);
			if (--bytes == 1) {
				/* indicate that next byte is the last one */
				outb(inb(smbus_base + SMBHSTCTL) | 0x20,
					 smbus_base + SMBHSTCTL);
			}
		}
	} while(status & 0x01);

	return bytes_read;
}

static int lsmbus_block_read(device_t dev, u8 cmd, u8 bytes, u8 *buf)
{
	u16 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);
	res = find_resource(pbus->dev, 0x20);
	return do_smbus_block_read(res->base, device, cmd, bytes, buf);
}


static struct smbus_bus_operations lops_smbus_bus = {
	.read_byte	= lsmbus_read_byte,
	.write_byte     = lsmbus_write_byte,
	.block_read     = lsmbus_block_read,
	.block_write    = lsmbus_block_write,
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
}

static struct device_operations smbus_ops = {
	.read_resources		= smbus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.scan_bus		= scan_static_bus,
	.enable			= i82801gx_enable,
	.ops_smbus_bus		= &lops_smbus_bus,
	.ops_pci		= &smbus_pci_ops,
};

/* 82801GB/GR/GDH/GBM/GHM/GU (ICH7/ICH7R/ICH7DH/ICH7-M/ICH7-M DH/ICH7-U) */
static const struct pci_driver i82801gx_smbus __pci_driver = {
	.ops	= &smbus_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x27da,
};
