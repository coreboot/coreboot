/*
 * This file is part of the superiotool project.
 *
 * Copyright (C) 2011 Jonathan A. Kollasch <jakllsch@kollasch.net>
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

#define DEVICE_ID_REG		0x20
#define DEVICE_REV_REG		0x21

static const struct superio_registers reg_table[] = {
	{0x0b, "SLB9635TT12", {
		{NOLDN, NULL,
			{0x20,0x21,0x26,0x27,EOT},
			{0x0b,0x00,NANA,NANA,EOT}},
		{0, NULL,
			{0x30,0x38,0x60,0x61,0x70,0x71,0xf1,0xf2,0xf3,0xf4,0xf5,EOT},
			{0x00,0x00,NANA,NANA,0x00,0x02,0xd1,0x15,0x0b,0x00,NANA,EOT}},
		{EOT}}},
	{EOT}
};

/* same as some SMSC */
static void enter_conf_mode_infineon(uint16_t port)
{
	OUTB(0x55, port);
}

static void exit_conf_mode_infineon(uint16_t port)
{
	OUTB(0xaa, port);
}

void probe_idregs_infineon(uint16_t port)
{
	uint8_t rev, devid;

	probing_for("Infineon", "", port);

	enter_conf_mode_infineon(port);

	devid = regval(port, DEVICE_ID_REG);
	rev = regval(port, DEVICE_REV_REG);

	if (superio_unknown(reg_table, devid)) {
		if (verbose)
			printf(NOTFOUND "id=0x%02x, rev=0x%02x\n", devid, rev);
		exit_conf_mode_infineon(port);
		return;
	}

	/* Attempt to prevent false matches on SMSC FDC37N972, see smsc.c */
	if (((regval(port, 0x27)<<8)|regval(port, 0x26)) != port) {
		if (verbose)
			printf(NOTFOUND "id=0x%02x, rev=0x%02x\n", devid, rev);
		exit_conf_mode_infineon(port);
		return;
	}

	printf("Found Infineon %s (id=0x%02x, rev=0x%02x) at 0x%x\n",
	       get_superio_name(reg_table, devid), devid, rev, port);
	chip_found = 1;

	dump_superio("Infineon", reg_table, port, devid, LDN_SEL);

	exit_conf_mode_infineon(port);
}

void print_infineon_chips(void)
{
	print_vendor_chips("Infineon", reg_table);
}
