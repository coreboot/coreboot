/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
 * Copyright (C) 2006,2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#include <console.h>
#include <io.h>
#include <device/device.h>
#include <device/pci.h>
#include "mcp55.h"
#define pci_read_config32(bus, dev, where) pci_cf8_conf1.read32(NULL, bus, dev, where)
#define pci_write_config32(bus, dev, where, what) pci_cf8_conf1.write32(NULL, bus, dev, where, what)
#define pci_read_config16(bus, dev, where) pci_cf8_conf1.read32(NULL, bus, dev, where)
#define pci_write_config16(bus, dev, where, what) pci_cf8_conf1.write32(NULL, bus, dev, where, what)
#include "mcp55_smbus.h"

#define SMBUS0_IO_BASE	0x1000
#define SMBUS1_IO_BASE	(0x1000+(1<<8))
/*SIZE 0x40 */

static void enable_smbus(void)
{
	u32 dev;
	dev = pci_locate_device(PCI_ID(0x10de, 0x0368), 0);
#if 0
	if (dev == PCI_DEV_INVALID) {
		die("SMBUS controller not found\r\n");
	}

	printk(BIOS_DEBUG, "SMBus controller enabled\r\n");
#endif
	/* set smbus iobase */
	pci_write_config32(0, dev, 0x20, SMBUS0_IO_BASE | 1);
	pci_write_config32(0, dev, 0x24, SMBUS1_IO_BASE | 1);
	/* Set smbus iospace enable */
	pci_write_config16(0, dev, 0x4, 0x01);
	/* clear any lingering errors, so the transaction will run */
	outb(inb(SMBUS0_IO_BASE + SMBHSTSTAT), SMBUS0_IO_BASE + SMBHSTSTAT);
	outb(inb(SMBUS1_IO_BASE + SMBHSTSTAT), SMBUS1_IO_BASE + SMBHSTSTAT);
}

static u8 smbus_recv_byte(u8 device)
{
	return do_smbus_recv_byte(SMBUS0_IO_BASE, device);
}

static u8 smbus_send_byte(u8 device,u8 val)
{
	return do_smbus_send_byte(SMBUS0_IO_BASE, device, val);
}

static u8 smbus_read_byte(u8 device, u8 address)
{
	return do_smbus_read_byte(SMBUS0_IO_BASE, device, address);
}

static u8 smbus_write_byte(u8 device, u8 address, u8 val)
{
	return do_smbus_write_byte(SMBUS0_IO_BASE, device, address, val);
}

static u8 smbusx_recv_byte(u8 smb_index, u8 device)
{
	return do_smbus_recv_byte(SMBUS0_IO_BASE + (smb_index<<8), device);
}

static u8 smbusx_send_byte(u8 smb_index, u8 device, u8 val)
{
	return do_smbus_send_byte(SMBUS0_IO_BASE + (smb_index<<8), device, val);
}

static u8 smbusx_read_byte(u8 smb_index, u8 device, u8 address)
{
	return do_smbus_read_byte(SMBUS0_IO_BASE + (smb_index<<8), device, address);
}

static u8 smbusx_write_byte(u8 smb_index, u8 device, u8 address, u8 val)
{
	return do_smbus_write_byte(SMBUS0_IO_BASE + (smb_index<<8), device, address, val);
}

