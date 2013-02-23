/*
 * This file is part of the superiotool project.
 *
 * Copyright (C) 2011 Ruud Schramp <schramp@holmes.nl>
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
	/*
	 * Note: These register defaults are based on educated guessing,
	 *       take them with a grain of salt.
	 *
	 * TODO: Don't know the ID registers yet: 0x21 probably is not an ID
	 * register as it is being set in the BIOS. For now still use as there
	 * is no known alternative.
	 */
	{0x02c0, "SE-SM 4210-P01", {
		{NOLDN, NULL,
			{0x1f,0x20,0x21,0x22,0x23,0x2c,0x2d,0x2e,EOT},
			{NANA,0x02,0xc0,0x00,0x00,RSVD,RSVD,RSVD,EOT}},
		{0x0, "UNKNOWN",
			{0x30,0x60,0x61,0x70,0xf0,0xf1,0xf2,EOT},
			{NANA,NANA,NANA,NANA,NANA,NANA,NANA,EOT}},
		{0x1, "COM2",
			{0x30,0x60,0x61,0x70,0xf0,0xf1,0xf2,EOT},
			{0x00,0x02,0xf8,0x03,0x00,0x00,0x0c,EOT}},
		{0x2, "COM1",
			{0x30,0x60,0x61,0x70,0xf0,0xf1,0xf2,EOT},
			{0x00,0x03,0xf8,0x04,0x00,0x00,0x0c,EOT}},
		{0x3, "UNKNOWN",
			{0x30,0x60,0x61,0x70,0xf0,0xf1,0xf2,EOT},
			{NANA,NANA,NANA,NANA,NANA,NANA,NANA,EOT}},
		{0x4, "UNKNOWN",
			{0x30,0x60,0x61,0x70,0xf0,0xf1,0xf2,EOT},
			{NANA,NANA,NANA,NANA,NANA,NANA,NANA,EOT}},
		{0x5, "UNKNOWN",
			{0x30,0x60,0x61,0x70,0xf0,0xf1,0xf2,EOT},
			{NANA,NANA,NANA,NANA,NANA,NANA,NANA,EOT}},
		{0x6, "UNKNOWN",
			{0x30,0x60,0x61,0x70,0xf0,0xf1,0xf2,EOT},
			{NANA,NANA,NANA,NANA,NANA,NANA,NANA,EOT}},
		{0x7, "UNKNOWN",
			{0x30,0x60,0x61,0x70,0xf0,0xf1,0xf2,EOT},
			{NANA,NANA,NANA,NANA,NANA,NANA,NANA,EOT}},
		{EOT}}},
	{EOT}
};

static void enter_conf_mode_serverengines(uint16_t port)
{
	OUTB(0x5a, port);
}

static void exit_conf_mode_serverengines(uint16_t port)
{
	OUTB(0xa5, port);
}

void probe_idregs_serverengines(uint16_t port)
{
	uint16_t id;
	uint8_t rev;

	probing_for("Server Engines", "", port);

	enter_conf_mode_serverengines(port);

	id = regval(port, DEVICE_ID_BYTE1_REG) << 8;
	id |= regval(port, DEVICE_ID_BYTE2_REG);

	/* TODO: Not documented/available on ServerEngines. */
	rev = regval(port, DEVICE_REV_REG);

	if (superio_unknown(reg_table, id)) {
		if (verbose)
			printf(NOTFOUND "id=0x%04x, rev=0x%02x\n", id, rev);
		exit_conf_mode_serverengines(port);
		return;
	}

	printf("Found Server Engines %s (id=0x%04x, rev=0x%02x) at 0x%x\n",
	       get_superio_name(reg_table, id), id, rev, port);
	chip_found = 1;

	dump_superio("Server Engines", reg_table, port, id, LDN_SEL);

	exit_conf_mode_serverengines(port);
}

void print_serverengines_chips(void)
{
	print_vendor_chips("Server Engines", reg_table);
}
