/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 Corey Osgood <corey_osgood@verizon.net>
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
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

#include <device/pci_ids.h>
#include "vt8237r.h"

#define VT8237R_POWER_WELL		0x94
#define VT8237R_SMBUS_IO_BASE_REG	0xd0
#define VT8237R_SMBUS_HOST_CONF		0xd2

#define SMBHSTSTAT	(VT8237R_SMBUS_IO_BASE + 0x0)
#define SMBSLVSTAT	(VT8237R_SMBUS_IO_BASE + 0x1)
#define SMBHSTCTL	(VT8237R_SMBUS_IO_BASE + 0x2)
#define SMBHSTCMD	(VT8237R_SMBUS_IO_BASE + 0x3)
#define SMBXMITADD	(VT8237R_SMBUS_IO_BASE + 0x4)
#define SMBHSTDAT0	(VT8237R_SMBUS_IO_BASE + 0x5)

#define HOST_RESET 		0xff
/* 1 in the 0 bit of SMBHSTADD states to READ. */
#define READ_CMD		0x01
#define SMBUS_TIMEOUT		(100 * 1000 * 10)
#define I2C_TRANS_CMD		0x40
#define CLOCK_SLAVE_ADDRESS	0x69

#if DEBUG_SMBUS == 1
#define PRINT_DEBUG(x)		print_debug(x)
#define PRINT_DEBUG_HEX16(x)	print_debug_hex16(x)
#else
#define PRINT_DEBUG(x)
#define PRINT_DEBUG_HEX16(x)
#endif

#define SMBUS_DELAY() inb(0x80)

static void smbus_print_error(u8 host_status, int loops)
{
	/* Check if there actually was an error. */
	if (host_status == 0x00 || host_status == 0x40 ||
	    host_status == 0x42)
		return;

	if (loops >= SMBUS_TIMEOUT)
		print_err("SMBus timeout\r\n");
	if (host_status & (1 << 4))
		print_err("Interrupt/SMI# was Failed Bus Transaction\r\n");
	if (host_status & (1 << 3))
		print_err("Bus error\r\n");
	if (host_status & (1 << 2))
		print_err("Device error\r\n");
	if (host_status & (1 << 1))
		print_err("Interrupt/SMI# was Successful Completion\r\n");
	if (host_status & (1 << 0))
		print_err("Host busy\r\n");
}

static void smbus_wait_until_ready(void)
{
	int loops;

	PRINT_DEBUG("Waiting until SMBus ready\r\n");

	loops = 0;
	/* Yes, this is a mess, but it's the easiest way to do it. */
	while ((inb(SMBHSTSTAT) & 1) == 1 && loops <= SMBUS_TIMEOUT)
		++loops;
	smbus_print_error(inb(SMBHSTSTAT), loops);
}

static void smbus_reset(void)
{
	outb(HOST_RESET, SMBHSTSTAT);
	/* Datasheet says we have to read it to take ownership of SMBus. */
	inb(SMBHSTSTAT);

	PRINT_DEBUG("After reset status: ");
	PRINT_DEBUG_HEX16(inb(SMBHSTSTAT));
	PRINT_DEBUG("\r\n");
}

u8 smbus_read_byte(u32 dimm, u32 offset)
{
	u32 val;

	PRINT_DEBUG("DIMM ");
	PRINT_DEBUG_HEX16(dimm);
	PRINT_DEBUG(" OFFSET ");
	PRINT_DEBUG_HEX16(offset);
	PRINT_DEBUG("\r\n");

	smbus_reset();
	/* Clear host data port. */
	outb(0x00, SMBHSTDAT0);
	SMBUS_DELAY();
	smbus_wait_until_ready();

	/* Actual addr to reg format. */
	dimm = (dimm << 1);
	dimm |= 1;
	outb(dimm, SMBXMITADD);
	outb(offset, SMBHSTCMD);
	/* Start transaction, byte data read. */
	outb(0x48, SMBHSTCTL);

	SMBUS_DELAY();

	smbus_wait_until_ready();

	val = inb(SMBHSTDAT0);
	PRINT_DEBUG("Read: ");
	PRINT_DEBUG_HEX16(val);
	PRINT_DEBUG("\r\n");

	/* Probably don't have to do this, but it can't hurt. */
	smbus_reset();

	/* Can I just "return inb(SMBHSTDAT0)"? */
	return val;
}

void enable_smbus(void)
{
	device_t dev;

	/* Power management controller */
	dev = pci_locate_device(PCI_ID(PCI_VENDOR_ID_VIA,
				       PCI_DEVICE_ID_VIA_VT8237R_LPC), 0);

	if (dev == PCI_DEV_INVALID)
		die("Power Management Controller not found\r\n");

	/* 7 = SMBus Clock from RTC 32.768KHz
	 * 5 = Internal PLL reset from susp
	 */
	pci_write_config8(dev, VT8237R_POWER_WELL, 0xa0);

	/* Enable SMBus */
	pci_write_config16(dev, VT8237R_SMBUS_IO_BASE_REG,
				VT8237R_SMBUS_IO_BASE | 0x1);

	/* SMBus Host Configuration, enable */
	pci_write_config8(dev, VT8237R_SMBUS_HOST_CONF, 0x01);

	/* Make it work for I/O. */
	pci_write_config16(dev, PCI_COMMAND, PCI_COMMAND_IO);

	smbus_reset();

	/* Reset the internal pointer. */
	inb(SMBHSTCTL);
}
