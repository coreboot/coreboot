/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <arch/io.h>
#include <delay.h>
#include <stdint.h>
#include "ec.h"

static u16 ec_cmd_reg = EC_SC;
static u16 ec_data_reg = EC_DATA;

int send_ec_command(u8 command)
{
	int timeout;

	timeout = 0x7ff;
	while ((inb(ec_cmd_reg) & EC_IBF) && --timeout) {
		udelay(10);
		if ((timeout & 0xff) == 0)
			printk(BIOS_SPEW, ".");
	}
	if (!timeout) {
		printk(BIOS_DEBUG, "Timeout while sending command 0x%02x to EC!\n",
				command);
		// return -1;
	}

	udelay(10);

	outb(command, ec_cmd_reg);
	return 0;
}

int send_ec_data(u8 data)
{
	int timeout;

	timeout = 0x7ff;
	while ((inb(ec_cmd_reg) & EC_IBF) && --timeout) { // wait for IBF = 0
		udelay(10);
		if ((timeout & 0xff) == 0)
			printk(BIOS_SPEW, ".");
	}
	if (!timeout) {
		printk(BIOS_DEBUG, "Timeout while sending data 0x%02x to EC!\n",
				data);
		// return -1;
	}

	udelay(10);

	outb(data, ec_data_reg);

	return 0;
}

int send_ec_data_nowait(u8 data)
{
	outb(data, ec_data_reg);

	return 0;
}

u8 recv_ec_data(void)
{
	int timeout;
	u8 data;

	timeout = 0x7fff;
	while (--timeout) { // Wait for OBF = 1
		if (inb(ec_cmd_reg) & EC_OBF) {
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

	udelay(10);

	data = inb(ec_data_reg);
	printk(BIOS_SPEW, "%s: 0x%02x\n", __func__, data);

	return data;
}

void ec_clear_out_queue(void)
{
	int timeout = 0x7fff;
	printk(BIOS_SPEW, "Clearing EC output queue...\n");
	while (--timeout && (inb(ec_cmd_reg) & EC_OBF)) {
		u8 data = inb(ec_data_reg);
		printk(BIOS_SPEW, "Discarding a garbage byte: 0x%02x\n", data);
		udelay(10);
	}
	if (!timeout)
		printk(BIOS_ERR, "Timeout while clearing EC output queue!\n");
	else
		printk(BIOS_SPEW, "EC output queue has been cleared.\n");
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

u8 ec_status(void)
{
	return inb(ec_cmd_reg);
}

u8 ec_query(void)
{
	send_ec_command(0x84);
	return recv_ec_data();
}

void ec_set_bit(u8 addr, u8 bit)
{
	ec_write(addr, ec_read(addr) | (1 << bit));
}

void ec_clr_bit(u8 addr, u8 bit)
{
	ec_write(addr, ec_read(addr) &  ~(1 << bit));
}

void ec_set_ports(u16 cmd_reg, u16 data_reg)
{
	if (!ENV_HAS_DATA_SECTION)
		return;

	ec_cmd_reg = cmd_reg;
	ec_data_reg = data_reg;
}
