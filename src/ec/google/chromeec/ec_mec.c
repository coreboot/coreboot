/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google Inc.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include "ec.h"
#include "ec_commands.h"

enum {
	/* 8-bit access */
	ACCESS_TYPE_BYTE = 0x0,
	/* 16-bit access */
	ACCESS_TYPE_WORD = 0x1,
	/* 32-bit access */
	ACCESS_TYPE_LONG = 0x2,
	/*
	 * 32-bit access, read or write of MEC_EMI_EC_DATA_B3 causes the
	 * EC data register to be incremented.
	 */
	ACCESS_TYPE_LONG_AUTO_INCREMENT = 0x3,
};

/* EMI registers are relative to base */
#define MEC_EMI_HOST_TO_EC	(MEC_EMI_BASE + 0)
#define MEC_EMI_EC_TO_HOST	(MEC_EMI_BASE + 1)
#define MEC_EMI_EC_ADDRESS_B0	(MEC_EMI_BASE + 2)
#define MEC_EMI_EC_ADDRESS_B1	(MEC_EMI_BASE + 3)
#define MEC_EMI_EC_DATA_B0	(MEC_EMI_BASE + 4)
#define MEC_EMI_EC_DATA_B1	(MEC_EMI_BASE + 5)
#define MEC_EMI_EC_DATA_B2	(MEC_EMI_BASE + 6)
#define MEC_EMI_EC_DATA_B3	(MEC_EMI_BASE + 7)

/*
 * cros_ec_lpc_mec_emi_write_address
 *
 * Initialize EMI read / write at a given address.
 *
 * @addr:        Starting read / write address
 * @access_mode: Type of access, typically 32-bit auto-increment
 */
static void mec_emi_write_address(u16 addr, u8 access_mode)
{
	/* Address relative to start of EMI range */
	addr -= MEC_EMI_RANGE_START;
	outb((addr & 0xfc) | access_mode, MEC_EMI_EC_ADDRESS_B0);
	outb((addr >> 8) & 0x7f, MEC_EMI_EC_ADDRESS_B1);
}

/*
 * mec_io_bytes - Read / write bytes to MEC EMI port
 *
 * @write:   1 on write operation, 0 on read
 * @port:    Base read / write address
 * @length:  Number of bytes to read / write
 * @buf:     Destination / source buffer
 * @csum:    Optional parameter, sums data transferred
 *
 */
void mec_io_bytes(int write, u16 port, unsigned int length, u8 *buf, u8 *csum)
{
	int i = 0;
	int io_addr;
	u8 access_mode, new_access_mode;

	if (length == 0)
		return;

	/*
	 * Long access cannot be used on misaligned data since reading B0 loads
	 * the data register and writing B3 flushes it.
	 */
	if ((port & 0x3) || (length < 4))
		access_mode = ACCESS_TYPE_BYTE;
	else
		access_mode = ACCESS_TYPE_LONG_AUTO_INCREMENT;

	/* Initialize I/O at desired address */
	mec_emi_write_address(port, access_mode);

	/* Skip bytes in case of misaligned port */
	io_addr = MEC_EMI_EC_DATA_B0 + (port & 0x3);
	while (i < length) {
		while (io_addr <= MEC_EMI_EC_DATA_B3) {
			if (write)
				outb(buf[i], io_addr++);
			else
				buf[i] = inb(io_addr++);
			if (csum)
				*csum += buf[i];

			port++;
			/* Extra bounds check in case of misaligned length */
			if (++i == length)
				return;
		}

		/*
		 * Use long auto-increment access except for misaligned write,
		 * since writing B3 triggers the flush.
		 */
		if (length - i < 4 && write)
			new_access_mode = ACCESS_TYPE_BYTE;
		else
			new_access_mode = ACCESS_TYPE_LONG_AUTO_INCREMENT;
		if (new_access_mode != access_mode ||
		    access_mode != ACCESS_TYPE_LONG_AUTO_INCREMENT) {
			access_mode = new_access_mode;
			mec_emi_write_address(port, access_mode);
		}

		/* Access [B0, B3] on each loop pass */
		io_addr = MEC_EMI_EC_DATA_B0;
	}
}
