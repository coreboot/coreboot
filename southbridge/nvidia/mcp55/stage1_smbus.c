/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
 * Copyright (C) 2006,2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
 * Copyright (C) 2008 Ronald G. Minnich (for v3)
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
#include "stage1.h"
#include "mcp55_smbus.h"

#define SMBUS0_IO_BASE	0x1000
#define SMBUS1_IO_BASE	(0x1000+(1<<8))
/*SIZE 0x40 */

int smbus_wait_until_ready(u16 smbus_io_base)
{
	unsigned long loops;
	loops = SMBUS_TIMEOUT;
	do {
		unsigned char val;
		smbus_delay();
		val = inb(smbus_io_base + SMBHSTSTAT);
		val &= 0x1f;
		if (val == 0) {
			return 0;
		}
		outb(val,smbus_io_base + SMBHSTSTAT);
	} while(--loops);
	return -2;
}

int smbus_wait_until_done(u16 smbus_io_base)
{
	unsigned long loops;
	loops = SMBUS_TIMEOUT;
	do {
		unsigned char val;
		smbus_delay();

		val = inb(smbus_io_base + SMBHSTSTAT);
		if ( (val & 0xff) != 0) {
			return 0;
		}
	} while(--loops);
	return -3;
}

int do_smbus_recv_byte(u16 smbus_io_base, u8 device)
{
	u8 global_status_register;
	u8 byte;

	/* set the device I'm talking too */
	outb(((device & 0x7f) << 1)|1 , smbus_io_base + SMBXMITADD);
	smbus_delay();

	/* byte data recv */
	outb(0x05, smbus_io_base + SMBHSTPRTCL);
	smbus_delay();

	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return -3;
	}

	global_status_register = inb(smbus_io_base + SMBHSTSTAT) & 0x80; /* lose check */

	/* read results of transaction */
	byte = inb(smbus_io_base + SMBHSTCMD);

	if (global_status_register != 0x80) { // loose check, otherwise it should be 0
		return -1;
	}
	return byte;
}

int do_smbus_send_byte(u16 smbus_io_base, u8 device, u8 val)
{
	u8 global_status_register;

	outb(val, smbus_io_base + SMBHSTDAT0);
	smbus_delay();

	/* set the command... */
	outb(val, smbus_io_base + SMBHSTCMD);
	smbus_delay();

	/* set the device I'm talking too */
	outb(((device & 0x7f) << 1) | 0, smbus_io_base + SMBXMITADD);
	smbus_delay();

	/* set up for a byte data write */
	outb(0x04, smbus_io_base + SMBHSTPRTCL);
	smbus_delay();

	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return -3;
	}
	global_status_register = inb(smbus_io_base + SMBHSTSTAT) & 0x80; /* lose check */;

	if (global_status_register != 0x80) {
		return -1;
	}
	return 0;
}

int do_smbus_read_byte(u16 smbus_io_base, u8 device, u8 address)
{
	u8 global_status_register;
	u8 byte;

	/* set the device I'm talking too */
	outb(((device & 0x7f) << 1)|1 , smbus_io_base + SMBXMITADD);
	smbus_delay();
	/* set the command/address... */
	outb(address & 0xff, smbus_io_base + SMBHSTCMD);
	smbus_delay();
	/* byte data read */
	outb(0x07, smbus_io_base + SMBHSTPRTCL);
	smbus_delay();

	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return -3;
	}

	global_status_register = inb(smbus_io_base + SMBHSTSTAT) & 0x80; /* lose check */

	/* read results of transaction */
	byte = inb(smbus_io_base + SMBHSTDAT0);

	if (global_status_register != 0x80) { // lose check, otherwise it should be 0
		return -1;
	}
	return byte;
}

int do_smbus_write_byte(u16 smbus_io_base, u8 device, u8 address, u8 val)
{
	u8 global_status_register;

	outb(val, smbus_io_base + SMBHSTDAT0);
	smbus_delay();

	/* set the device I'm talking too */
	outb(((device & 0x7f) << 1) | 0, smbus_io_base + SMBXMITADD);
	smbus_delay();

	outb(address & 0xff, smbus_io_base + SMBHSTCMD);
	smbus_delay();

	/* set up for a byte data write */
	outb(0x06, smbus_io_base + SMBHSTPRTCL);
	smbus_delay();

	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return -3;
	}
	global_status_register = inb(smbus_io_base + SMBHSTSTAT) & 0x80; /* lose check */;

	if (global_status_register != 0x80) {
		return -1;
	}
	return 0;
}


void enable_smbus(void)
{
	u32 bdf;
	if (!pci_locate_device(0x10de, 0x0368, &bdf)) {
		die("SMBUS controller not found\r\n");
	}

	printk(BIOS_DEBUG, "SMBus controller enabled\n");
	/* set smbus iobase */
	pci_write_config32(bdf, 0x20, SMBUS0_IO_BASE | 1);
	pci_write_config32(bdf, 0x24, SMBUS1_IO_BASE | 1);
	/* Set smbus iospace enable */
	pci_write_config16(bdf, 0x4, 0x01);
	/* clear any lingering errors, so the transaction will run */
	outb(inb(SMBUS0_IO_BASE + SMBHSTSTAT), SMBUS0_IO_BASE + SMBHSTSTAT);
	outb(inb(SMBUS1_IO_BASE + SMBHSTSTAT), SMBUS1_IO_BASE + SMBHSTSTAT);
}

u8 smbus_recv_byte(u8 device)
{
	return do_smbus_recv_byte(SMBUS0_IO_BASE, device);
}

u8 smbus_send_byte(u8 device,u8 val)
{
	return do_smbus_send_byte(SMBUS0_IO_BASE, device, val);
}

u8 smbus_read_byte(u8 device, u8 address)
{
	return do_smbus_read_byte(SMBUS0_IO_BASE, device, address);
}

u8 smbus_write_byte(u8 device, u8 address, u8 val)
{
	return do_smbus_write_byte(SMBUS0_IO_BASE, device, address, val);
}

u8 smbusx_recv_byte(u8 smb_index, u8 device)
{
	return do_smbus_recv_byte(SMBUS0_IO_BASE + (smb_index<<8), device);
}

u8 smbusx_send_byte(u8 smb_index, u8 device, u8 val)
{
	return do_smbus_send_byte(SMBUS0_IO_BASE + (smb_index<<8), device, val);
}

u8 smbusx_read_byte(u8 smb_index, u8 device, u8 address)
{
	return do_smbus_read_byte(SMBUS0_IO_BASE + (smb_index<<8), device, address);
}

u8 smbusx_write_byte(u8 smb_index, u8 device, u8 address, u8 val)
{
	return do_smbus_write_byte(SMBUS0_IO_BASE + (smb_index<<8), device, address, val);
}

