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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "smbus.h"

#define SMBUS_IO_BASE 0x1000

static void enable_smbus(void)
{
	device_t dev;
	dev = pci_locate_device(PCI_ID(0x1166, 0x0205), 0); // 0x0201?

	if (dev == PCI_DEV_INVALID) {
		die("SMBUS controller not found\n");
	}

	print_debug("SMBus controller enabled\n");
	/* set smbus iobase */
	pci_write_config32(dev, 0x90, SMBUS_IO_BASE | 1);
	/* Set smbus iospace enable */
	pci_write_config8(dev, 0xd2, 0x03);
	/* clear any lingering errors, so the transaction will run */
	outb(inb(SMBUS_IO_BASE + SMBHSTSTAT), SMBUS_IO_BASE + SMBHSTSTAT);
}

static inline int smbus_recv_byte(unsigned device)
{
        return do_smbus_recv_byte(SMBUS_IO_BASE, device);
}

static inline int smbus_send_byte(unsigned device, unsigned char val)
{
        return do_smbus_send_byte(SMBUS_IO_BASE, device, val);
}

static inline int smbus_read_byte(unsigned device, unsigned address)
{
        return do_smbus_read_byte(SMBUS_IO_BASE, device, address);
}

static inline int smbus_write_byte(unsigned device, unsigned address, unsigned char val)
{
        return do_smbus_write_byte(SMBUS_IO_BASE, device, address, val);
}
