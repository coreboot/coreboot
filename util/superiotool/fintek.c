/*
 * This file is part of the superiotool project.
 *
 * Copyright (C) 2006 coresystems GmbH <info@coresystems.de>
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

#define DEVICE_ID_BYTE1_REG	0x20
#define DEVICE_ID_BYTE2_REG	0x21

#define VENDOR_ID_BYTE1_REG	0x23
#define VENDOR_ID_BYTE2_REG	0x24

#define FINTEK_VENDOR_ID	0x3419

const static struct superio_registers reg_table[] = {
	{0x0106, "F71862FG", {
		{EOT}}},
	{0x4103, "F71872F/FG / F71806F/FG", {	/* Same ID? Datasheet typo? */
		{EOT}}},
	{0x4105, "F71882FG/F71883FG", {		/* Same ID? Datasheet typo? */
		{EOT}}},
	{0x0604, "F71805F/FG", {
		/* We assume reserved bits are read as 0. */
		{NOLDN, NULL,
			{0x07,0x20,0x21,0x23,0x24,0x25,0x26,0x27,0x28,
			 0x29,EOT},
			{NANA,0x04,0x06,0x19,0x34,0x00,0x00,0x3f,0x08,
			 0x00,EOT}},
		{0x0, "Floppy",
			{0x30,0x60,0x61,0x70,0x74,0xf0,0xf2,0xf4,EOT},
			{0x01,0x03,0xf0,0x06,0x02,0x0e,0x03,0x00,EOT}},
		{0x1, "COM1",
			{0x30,0x60,0x61,0x70,0xf0,EOT},
			{0x01,0x03,0xf8,0x04,0x00,EOT}},
		{0x2, "COM2",
			{0x30,0x60,0x61,0x70,0xf0,0xf1,EOT},
			{0x01,0x02,0xf8,0x03,0x00,0x04,EOT}},
		{0x3, "Parallel port",
			{0x30,0x60,0x61,0x70,0x74,0xf0,EOT},
			{0x01,0x03,0x78,0x07,0x03,0x42,EOT}},
		{0x4, "Hardware monitor",
			{0x30,0x60,0x61,0x70,EOT},
			{0x00,0x02,0x95,0x00,EOT}},
		{0x6, "GPIO",
			{0x70,0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,
			 0xe9,0xf0,0xf1,0xf3,0xf4,EOT},
			{0x00,0x00,0x00,NANA,0x00,0x00,0x00,0x00,0x00,0x00,
			 0x00,0x00,NANA,0x00,NANA,EOT}},
		{0xa, "PME",
			{0x30,0xf0,0xf1,EOT},
			{0x00,0x00,0x00,EOT}},
		{EOT}}},
	{EOT}
};

void probe_idregs_fintek(uint16_t port)
{
	uint16_t vid, did;

	probing_for("Fintek", "", port);

	enter_conf_mode_winbond_fintek_ite_8787(port);

	did = regval(port, DEVICE_ID_BYTE1_REG);
	did |= (regval(port, DEVICE_ID_BYTE2_REG) << 8);

	vid = regval(port, VENDOR_ID_BYTE1_REG);
	vid |= (regval(port, VENDOR_ID_BYTE2_REG) << 8);

	if (vid != FINTEK_VENDOR_ID || superio_unknown(reg_table, did)) {
		if (verbose)
			printf(NOTFOUND "vid=0x%04x, id=0x%04x\n", vid, did);
		exit_conf_mode_winbond_fintek_ite_8787(port);
		return;
	}

	printf("Found Fintek %s (vid=0x%04x, id=0x%04x) at 0x%x\n",
	       get_superio_name(reg_table, did), vid, did, port);
	chip_found = 1;

	dump_superio("Fintek", reg_table, port, did);

	exit_conf_mode_winbond_fintek_ite_8787(port);
}

