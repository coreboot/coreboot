/*
 * This file is part of the ectool project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#ifndef __NetBSD__
#include <sys/io.h>
#endif
#include "ec.h"

#ifdef __NetBSD__
#include <machine/sysarch.h>
static uint8_t inb(unsigned port)
{
	uint8_t data;
	__asm volatile("inb %w1,%0" : "=a" (data) : "d" (port));
	return data;
}
static __inline void outb(uint8_t data, unsigned port)
{
	__asm volatile("outb %0,%w1" : : "a" (data), "d" (port));
}
#endif

extern int verbose;

#define debug(x...) if (verbose) printf(x)

int send_ec_command(uint8_t command)
{
	int timeout;

	timeout = 0x7ff;
	while ((inb(EC_SC) & EC_IBF) && --timeout) {
		usleep(10);
		if ((timeout & 0xff) == 0)
			debug(".");
	}
	if (!timeout) {
		debug("Timeout while sending command 0x%02x to EC!\n",
		       command);
		// return -1;
	}

	outb(command, EC_SC);
	return 0;
}

int send_ec_data(uint8_t data)
{
	int timeout;

	timeout = 0x7ff;
	while ((inb(EC_SC) & EC_IBF) && --timeout) {	// wait for IBF = 0
		usleep(10);
		if ((timeout & 0xff) == 0)
			debug(".");
	}
	if (timeout) {
		debug("Timeout while sending data 0x%02x to EC!\n", data);
		// return -1;
	}

	outb(data, EC_DATA);

	return 0;
}

int send_ec_data_nowait(uint8_t data)
{
	outb(data, EC_DATA);

	return 0;
}

uint8_t recv_ec_data(void)
{
	int timeout;
	uint8_t data;

	timeout = 0x7fff;
	while (--timeout) {	// Wait for OBF = 1
		if (inb(EC_SC) & EC_OBF) {
			break;
		}
		usleep(10);
		if ((timeout & 0xff) == 0)
			debug(".");
	}
	if (!timeout) {
		debug("\nTimeout while receiving data from EC!\n");
		// return -1;
	}

	data = inb(EC_DATA);
	debug("recv_ec_data: 0x%02x\n", data);

	return data;
}

uint8_t ec_read(uint8_t addr)
{
	send_ec_command(RD_EC);
	send_ec_data(addr);

	return recv_ec_data();
}

uint8_t ec_ext_read(uint16_t addr)
{
	send_ec_command(WR_EC);
	send_ec_data(0x02);
	send_ec_data(addr & 0xff);
	send_ec_command(RX_EC);
	send_ec_data(addr >> 8);

	return recv_ec_data();
}

int ec_ext_write(uint16_t addr, uint8_t data)
{
	send_ec_command(WR_EC);
	send_ec_data(0x02);
	send_ec_data(addr & 0xff);
	send_ec_command(WX_EC);
	send_ec_data(addr >> 8);

	return send_ec_data(data);
}

int ec_write(uint8_t addr, uint8_t data)
{
	send_ec_command(WR_EC);
	send_ec_data(addr);

	return send_ec_data(data);
}

uint8_t ec_idx_read(uint16_t addr)
{
	uint16_t lpc_idx = 0x380;

	outb(addr & 0xff, lpc_idx + 2);
	outb(addr >> 8, lpc_idx + 1);

	return inb(lpc_idx + 3);
}

uint8_t ec_query(void)
{
	send_ec_command(QR_EC);
	return recv_ec_data();
}
