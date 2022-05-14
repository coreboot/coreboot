/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <arch/io.h>
#include <delay.h>
#include <types.h>
#include "ec.h"

#define EC_POLL_DELAY_US	10
#define EC_SEND_TIMEOUT_US	20000	// 20ms
#define EC_RECV_TIMEOUT_US	320000	// 320ms

static u16 ec_cmd_reg = EC_SC;
static u16 ec_data_reg = EC_DATA;

/*
 * Poll the EC status/command register for a specified
 * state until the given timeout elapses.
 */
static int wait_ec_sc(int timeout_us, u8 mask, u8 state)
{
	while (timeout_us > 0 && (inb(ec_cmd_reg) & mask) != state) {
		udelay(EC_POLL_DELAY_US);
		timeout_us -= EC_POLL_DELAY_US;
	}

	return timeout_us > 0 ? 0 : -1;
}

bool ec_ready_send(int timeout_us)
{
	return wait_ec_sc(timeout_us, EC_IBF, 0) == 0;
}

bool ec_ready_recv(int timeout_us)
{
	return wait_ec_sc(timeout_us, EC_OBF, EC_OBF) == 0;
}

int send_ec_command(u8 command)
{
	return send_ec_command_timeout(command, EC_SEND_TIMEOUT_US);
}

int send_ec_command_timeout(u8 command, int timeout_us)
{
	if (!ec_ready_send(timeout_us)) {
		printk(BIOS_DEBUG, "Timeout while sending command 0x%02x to EC!\n",
				command);
		return -1;
	}

	outb(command, ec_cmd_reg);

	return 0;
}

int send_ec_data(u8 data)
{
	return send_ec_data_timeout(data, EC_SEND_TIMEOUT_US);
}

int send_ec_data_timeout(u8 data, int timeout_us)
{
	if (!ec_ready_send(timeout_us)) {
		printk(BIOS_DEBUG, "Timeout while sending data 0x%02x to EC!\n",
				data);
		return -1;
	}

	outb(data, ec_data_reg);

	return 0;
}

int recv_ec_data(void)
{
	return recv_ec_data_timeout(EC_RECV_TIMEOUT_US);
}

int recv_ec_data_timeout(int timeout_us)
{
	u8 data;

	if (!ec_ready_recv(timeout_us)) {
		printk(BIOS_DEBUG, "Timeout while receiving data from EC!\n");
		return -1;
	}

	data = inb(ec_data_reg);
	printk(BIOS_SPEW, "%s: 0x%02x\n", __func__, data);

	return data;
}

void ec_clear_out_queue(void)
{
	int timeout = EC_RECV_TIMEOUT_US;
	printk(BIOS_SPEW, "Clearing EC output queue...\n");
	while (timeout > 0 && inb(ec_cmd_reg) & EC_OBF) {
		u8 data = inb(ec_data_reg);
		printk(BIOS_SPEW, "Discarding a garbage byte: 0x%02x\n", data);
		udelay(EC_POLL_DELAY_US);
		timeout -= EC_POLL_DELAY_US;
	}
	if (timeout <= 0)
		printk(BIOS_ERR, "Timeout while clearing EC output queue!\n");
	else
		printk(BIOS_SPEW, "EC output queue has been cleared.\n");
}

u8 ec_read(u8 addr)
{
	send_ec_command(RD_EC);
	send_ec_data(addr);

	return recv_ec_data();
}

int ec_write(u8 addr, u8 data)
{
	send_ec_command(WR_EC);
	send_ec_data(addr);
	return send_ec_data(data);
}

u8 ec_status(void)
{
	return inb(ec_cmd_reg);
}

u8 ec_query(void)
{
	send_ec_command(QR_EC);
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
