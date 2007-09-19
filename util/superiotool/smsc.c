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

void enter_conf_mode_smsc(uint16_t port)
{
	outb(0x55, port);
}

void exit_conf_mode_smsc(uint16_t port)
{
	outb(0xaa, port);
}

/* Note: The actual SMSC ID is 16 bits, but we must pass 32 bits here. */
void dump_smsc(uint16_t port, uint16_t id)
{
	switch (id) {
	case 0x28:
		dump_superio("SMSC", reg_table, port, id);
		break;
	default:
		printf("Unknown SMSC chip, id=0x%02x\n", id);
		break;
	}
}

void probe_idregs_smsc(uint16_t port)
{
	uint16_t id, rev;

	enter_conf_mode_smsc(port);

	/* Read device ID. */
	id = regval(port, DEVICE_ID_REG);
	if (id != 0x28) {	/* TODO: Support for other SMSC chips. */
		if (inb(port) != 0xff)
			printf("No Super I/O chip found at 0x%04x\n", port);
		else
			printf("Probing 0x%04x, failed (0x%02x), data returns 0x%02x\n", port, inb(port), inb(port + 1));
		return;
	}

	/* Read chip revision. */
	rev = regval(port, DEVICE_REV_REG);

	printf("Super I/O found at 0x%04x: id=0x%02x, rev=0x%02x\n",
	       port, id, rev);

	dump_smsc(port, id );

	exit_conf_mode_smsc(port);
}

