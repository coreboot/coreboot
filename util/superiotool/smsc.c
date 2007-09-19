/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
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

#include "superiotool.h"

#define DEVICE_ID_REG	0x0d
#define DEVICE_REV_REG	0x0e

const static struct superio_registers reg_table[] = {
	{0x28, "FDC37N769", {
		{NOLDN,
			{0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
			 0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,
			 0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,
			 0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,
			 0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,EOT},
			{0x28,0x9c,0x88,0x70,0x00,0x00,0xff,0x00,0x00,0x00,
			 0x00,0x00,0x02,0x28,NANA,0x00,0x00,0x80,RSVD,RSVD,
			 NANA,NANA,NANA,0x03,RSVD,RSVD,RSVD,RSVD,RSVD,RSVD,
			 0x80,0x00,0x3c,RSVD,RSVD,0x00,0x00,0x00,0x00,0x00,
			 0x00,0x00,RSVD,0x00,0x00,0x03,0x00,0x00,EOT}},
		{EOT}}},
	{EOT}
};

static void enter_conf_mode_smsc(uint16_t port)
{
	outb(0x55, port);
}

static void exit_conf_mode_smsc(uint16_t port)
{
	outb(0xaa, port);
}

void probe_idregs_smsc(uint16_t port)
{
	uint16_t id, rev;

	enter_conf_mode_smsc(port);

	/* Read device ID. */
	id = regval(port, DEVICE_ID_REG);
	if (id != 0x28) {	/* TODO: Support for other SMSC chips. */
		no_superio_found(port);
		return;
	}

	/* Read chip revision. */
	rev = regval(port, DEVICE_REV_REG);

	printf("Found SMSC %s Super I/O (id=0x%02x, rev=0x%02x) at port=0x%04x\n",
	       get_superio_name(reg_table, id), id, rev, port);

	if (dump)
		dump_superio("SMSC", reg_table, port, id);

	exit_conf_mode_smsc(port);
}

