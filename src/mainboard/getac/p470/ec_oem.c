/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <arch/io.h>
#include <delay.h>
#include <ec/acpi/ec.h>
#include "ec_oem.h"

int send_ec_oem_command(u8 command)
{
	int timeout;

	timeout = 0x7ff;
	while ((inb(EC_OEM_SC) & EC_IBF) && --timeout) {
		udelay(10);
		if ((timeout & 0xff) == 0)
			printk(BIOS_SPEW, ".");
	}
	if (!timeout) {
		printk(BIOS_DEBUG, "Timeout while sending OEM command 0x%02x to EC!\n",
				command);
		// return -1;
	}

	outb(command, EC_OEM_SC);
	return 0;
}

int send_ec_oem_data(u8 data)
{
	int timeout;

	timeout = 0x7ff;
	while ((inb(EC_OEM_SC) & EC_IBF) && --timeout) { // wait for IBF = 0
		udelay(10);
		if ((timeout & 0xff) == 0)
			printk(BIOS_SPEW, ".");
	}
	if (!timeout) {
		printk(BIOS_DEBUG, "Timeout while sending OEM data 0x%02x to EC!\n",
				data);
		// return -1;
	}

	outb(data, EC_OEM_DATA);

	return 0;
}

u8 recv_ec_oem_data(void)
{
	int timeout;
	u8 data;

	timeout = 0x7fff;
	while (--timeout) { // Wait for OBF = 1
		if (inb(EC_OEM_SC) & EC_OBF) {
			break;
		}
		udelay(10);
		if ((timeout & 0xff) == 0)
			printk(BIOS_SPEW, ".");
	}
	if (!timeout) {
		printk(BIOS_DEBUG, "\nTimeout while receiving OEM data from EC!\n");
		// return -1;
	}

	data = inb(EC_OEM_DATA);
	// printk(BIOS_SPEW, "recv_ec_oem_data: 0x%02x\n", data);

	return data;
}

u8 ec_oem_read(u8 addr)
{
	send_ec_oem_command(0x80);
	send_ec_oem_data(addr);

	return recv_ec_oem_data();
}

int ec_oem_write(u8 addr, u8 data)
{
	send_ec_oem_command(0x81);
	send_ec_oem_data(addr);
	return send_ec_oem_data(data);
}

int ec_oem_dump_status(void)
{
	u8 ec_sc = inb(EC_OEM_SC);
	printk(BIOS_DEBUG, "Embedded Controller Status: ");
	if (ec_sc & (1 << 6)) printk(BIOS_DEBUG, "SMI_EVT ");
	if (ec_sc & (1 << 5)) printk(BIOS_DEBUG, "SCI_EVT ");
	if (ec_sc & (1 << 4)) printk(BIOS_DEBUG, "BURST ");
	if (ec_sc & (1 << 3)) printk(BIOS_DEBUG, "CMD ");
	if (ec_sc & (1 << 1)) printk(BIOS_DEBUG, "IBF ");
	if (ec_sc & (1 << 0)) printk(BIOS_DEBUG, "OBF ");
	printk(BIOS_DEBUG, "\n");

	return ec_sc;
}
