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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <arch/io.h>
#include <delay.h>
#include "ec.h"

int send_ec_command(u8 command)
{
	int timeout;

	timeout = 0x7ff;
	while ((inb(EC_SC) & EC_IBF) && --timeout) {
		udelay(10);
		if ((timeout & 0xff) == 0)
			printk(BIOS_SPEW, ".");
	}
	if (!timeout) {
		printk(BIOS_DEBUG, "Timeout while sending command 0x%02x to EC!\n",
				command);
		// return -1;
	}

	outb(command, EC_SC);
	return 0;
}

int send_ec_data(u8 data)
{
	int timeout;

	timeout = 0x7ff;
	while ((inb(EC_SC) & EC_IBF) && --timeout) { // wait for IBF = 0
		udelay(10);
		if ((timeout & 0xff) == 0)
			printk(BIOS_SPEW, ".");
	}
	if (!timeout) {
		printk(BIOS_DEBUG, "Timeout while sending data 0x%02x to EC!\n",
				data);
		// return -1;
	}

	outb(data, EC_DATA);

	return 0;
}

int send_ec_data_nowait(u8 data)
{
	outb(data, EC_DATA);

	return 0;
}

u8 recv_ec_data(void)
{
	int timeout;
	u8 data;

	timeout = 0x7fff;
	while (--timeout) { // Wait for OBF = 1
		if (inb(EC_SC) & EC_OBF) {
			break;
		}
		udelay(10);
		if ((timeout & 0xff) == 0)
			printk(BIOS_SPEW, ".");
	}
	if (!timeout) {
		printk(BIOS_DEBUG, "\nTimeout while receiving data from EC!\n");
		// return -1;
	}

	data = inb(EC_DATA);
	printk(BIOS_DEBUG, "recv_ec_data: 0x%02x\n", data);

	return data;
}

u8 ec_read(u8 addr)
{
	send_ec_command(0x80);
	send_ec_data(addr);

	return recv_ec_data();
}

int ec_write(u8 addr, u8 data)
{
	send_ec_command(0x81);
	send_ec_data(addr);
	return send_ec_data(data);
}

struct chip_operations ec_acpi_ops = {
	CHIP_NAME("ACPI Embedded Controller")
};
