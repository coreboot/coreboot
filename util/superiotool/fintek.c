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

static void dump_readable_fintek(uint16_t port, uint16_t did)
{
	if (!dump_readable)
		return;

	printf("Human-readable register dump:\n");

	printf("Flash write is %s.\n",
	       regval(port, 0x28) & 0x80 ? "enabled" : "disabled");
	printf("Flash control is 0x%04x.\n", regval(port, 0x28));
	printf("27=%02x\n", regval(port, 0x27));
	printf("29=%02x\n", regval(port, 0x29));
	printf("2a=%02x\n", regval(port, 0x2a));
	printf("2b=%02x\n", regval(port, 0x2b));

	/* Select UART 1. */
	regwrite(port, 0x07, 0x01);
	printf("UART1 is %s\n",
	       regval(port, 0x30) & 1 ? "enabled" : "disabled");
	printf("UART1 base=%02x%02x, irq=%02x, mode=%s\n", regval(port, 0x60),
	       regval(port, 0x61), regval(port, 0x70) & 0x0f,
	       regval(port, 0xf0) & 0x10 ? "RS485" : "RS232");

	/* Select UART 2. */
	regwrite(port, 0x07, 0x02);
	printf("UART2 is %s\n",
	       regval(port, 0x30) & 1 ? "enabled" : "disabled");
	printf("UART2 base=%02x%02x, irq=%02x, mode=%s\n", regval(port, 0x60),
	       regval(port, 0x61), regval(port, 0x70) & 0x0f,
	       regval(port, 0xf0) & 0x10 ? "RS485" : "RS232");

	/* Select parallel port. */
	regwrite(port, 0x07, 0x03);
	printf("PARPORT is %s\n",
	       regval(port, 0x30) & 1 ? "enabled" : "disabled");
	printf("PARPORT base=%02x%02x, irq=%02x\n", regval(port, 0x60),
	       regval(port, 0x61), regval(port, 0x70) & 0x0f);

	/* Select HW monitor. */
	regwrite(port, 0x07, 0x04);
	printf("HW monitor is %s\n",
	       regval(port, 0x30) & 1 ? "enabled" : "disabled");
	printf("HW monitor base=%02x%02x, irq=%02x\n", regval(port, 0x60),
	       regval(port, 0x61), regval(port, 0x70) & 0x0f);

	/* Select GPIO. */
	regwrite(port, 0x07, 0x05);
	printf("GPIO is %s\n", regval(port, 0x30) & 1 ? "enabled" : "disabled");
	printf
	    ("GPIO 70=%02x, e0=%02x, e1=%02x, e2=%02x, e3=%02x, e4=%02x, e5=%02x\n",
	     regval(port, 0x70), regval(port, 0xe0), regval(port, 0xe1),
	     regval(port, 0xe2), regval(port, 0xe3), regval(port, 0xe4),
	     regval(port, 0xe5));
	printf
	    ("GPIO e6=%02x, e7=%02x, e8=%02x, e9=%02x, f0=%02x, f1=%02x, f3=%02x, f4=%02x\n",
	     regval(port, 0xe6), regval(port, 0xe7), regval(port, 0xe8),
	     regval(port, 0xe9), regval(port, 0xf0), regval(port, 0xf1),
	     regval(port, 0xf3), regval(port, 0xf4));
	printf("GPIO f5=%02x, f6=%02x, f7=%02x, f8=%02x\n", regval(port, 0xf5),
	       regval(port, 0xf6), regval(port, 0xf7), regval(port, 0xf8));
}

void probe_idregs_fintek(uint16_t port)
{
	uint16_t vid, did;

	enter_conf_mode_winbond_fintek_ite_8787(port);

	did = regval(port, DEVICE_ID_BYTE1_REG);
	did |= (regval(port, DEVICE_ID_BYTE2_REG) << 8);

	vid = regval(port, VENDOR_ID_BYTE1_REG);
	vid |= (regval(port, VENDOR_ID_BYTE2_REG) << 8);

	if (vid != FINTEK_VENDOR_ID || superio_unknown(reg_table, did)) {
		no_superio_found("Fintek", "", port);
		exit_conf_mode_winbond_fintek_ite_8787(port);
		return;
	}

	printf("Found Fintek %s (vid=0x%04x, id=0x%04x) at port=0x%x\n",
	       get_superio_name(reg_table, did), vid, did, port);

	dump_superio("Fintek", reg_table, port, did);
	dump_readable_fintek(port, did);

	exit_conf_mode_winbond_fintek_ite_8787(port);
}

