/*
 * This file is part of the superiotool project.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "superiotool.h"

#define DEVICE_ID_BYTE1_REG	0x20
#define DEVICE_ID_BYTE2_REG	0x21

#define DEVICE_REV_REG		0x1f

static const struct superio_registers reg_table[] = {
	/* TODO: M5113 doesn't seem to have ID registers? */
	{0x5315, "M1535/M1535D/M1535+/M1535D+", {
		{NOLDN, NULL,
			{0x1f,0x20,0x21,0x22,0x23,0x2c,0x2d,0x2e,EOT},
			{NANA,0x53,0x15,0x00,0x00,RSVD,RSVD,RSVD,EOT}},
		{0x0, "Floppy",
			{0x30,0x60,0x61,0x70,0x74,0xf0,0xf1,0xf2,0xf4,EOT},
			{0x00,0x03,0xf0,0x06,0x02,0x08,0x00,0xff,0x00,EOT}},
		{0x3, "Parallel port",
			{0x30,0x60,0x61,0x70,0x74,0xf0,0xf1,EOT},
			{0x00,0x03,0x78,0x05,0x04,0x8c,0xc5,EOT}},
		{0x4, "COM1",
			{0x30,0x60,0x61,0x70,0xf0,0xf1,0xf2,EOT},
			{0x00,0x03,0xf8,0x04,0x00,0x00,0x0c,EOT}},
		{0x5, "COM2",
			{0x30,0x60,0x61,0x70,0x74,0xf0,0xf1,0xf2,EOT},
			{0x00,0x03,0xe8,0x09,0x04,0x80,0x00,0x0c,EOT}},
		{0x7, "Keyboard",
			{0x30,0x70,0x72,0xf0,EOT},
			{NANA,0x01,0x00,0x00,EOT}},
		{0x8, "COM3",
			{0x30,0x60,0x61,0x70,0xf0,0xf1,0xf2,EOT},
			{0x00,0x02,0xf8,0x03,0x00,0x00,0x0c,EOT}},
		{0xc, "Hotkey",
			{0x30,0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,EOT},
			{0x00,0x35,0x14,0x11,0x71,RSVD,0x05,EOT}},
		{EOT}}},
	{0x2351, "M512x", {
		{EOT}}},
	{EOT}
};

static void enter_conf_mode_ali(uint16_t port)
{
	OUTB(0x51, port);
	OUTB(0x23, port);
}

static void exit_conf_mode_ali(uint16_t port)
{
	OUTB(0xbb, port);
}

void probe_idregs_ali(uint16_t port)
{
	uint16_t id;
	uint8_t rev;

	probing_for("ALi", "", port);

	enter_conf_mode_ali(port);

	id = regval(port, DEVICE_ID_BYTE1_REG) << 8;
	id |= regval(port, DEVICE_ID_BYTE2_REG);

	/* TODO: Not documented/available on M512x (?) */
	rev = regval(port, DEVICE_REV_REG);

	if (superio_unknown(reg_table, id)) {
		if (verbose)
			printf(NOTFOUND "id=0x%04x, rev=0x%02x\n", id, rev);
		exit_conf_mode_ali(port);
		return;
	}

	printf("Found ALi %s (id=0x%04x, rev=0x%02x) at 0x%x\n",
	       get_superio_name(reg_table, id), id, rev, port);
	chip_found = 1;

	dump_superio("ALi", reg_table, port, id, LDN_SEL);

	exit_conf_mode_ali(port);
}

void print_ali_chips(void)
{
	print_vendor_chips("ALi", reg_table);
}
