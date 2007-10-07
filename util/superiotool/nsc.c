/*
 * This file is part of the superiotool project.
 *
 * Copyright (C) 2006 Ronald Minnich <rminnich@gmail.com>
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

#define CHIP_ID_REG	0x20	/* Super I/O ID (SID) / family */
#define CHIP_REV_REG	0x27	/* Super I/O revision ID (SRID) */

const static struct superio_registers reg_table[] = {
	{0xd0, "PC87371", {	/* From sensors-detect */
		{EOT}}},
	{0xdf, "PC97371", {	/* From sensors-detect */
		{EOT}}},
	{0xe1, "PC87360", {
		{EOT}}},
	{0xe2, "PC87351", {
		{NOLDN, NULL,
			{0x20,0x21,0x22,0x23,0x24,0x27,0x2e,EOT},
			{0xe2,0x11,0xa1,0x00,MISC,NANA,RSVD,EOT}},
		{0x0, "Floppy",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,0xf1,EOT},
			{0x00,0x03,0xf2,0x06,0x03,0x02,0x04,0x24,0x00,EOT}},
		{0x1, "Parallel port",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x02,0x78,0x07,0x02,0x04,0x04,0xf2,EOT}},
		{0x2, "COM2",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x02,0xf8,0x03,0x03,0x04,0x04,0x02,EOT}},
		{0x3, "COM1",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x03,0xf8,0x04,0x03,0x04,0x04,0x02,EOT}},
		{0x4, "System wake-up control (SWC)",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,EOT},
			{0x00,0x00,0x00,0x00,0x03,0x04,0x04,EOT}},
		{0x5, "Mouse",
			{0x30,0x70,0x71,0x74,0x75,EOT},
			{0x00,0x0c,0x02,0x04,0x04,EOT}},
		{0x6, "Keyboard",
			{0x30,0x60,0x61,0x62,0x63,0x70,0x71,0x74,0x75,
			 0xf0,EOT},
			{0x01,0x00,0x60,0x00,0x64,0x01,0x02,0x04,0x04,
			 0x40,EOT}},
		{0x7, "GPIO",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,0xf1,EOT},
			{0x00,0x00,0x00,0x00,0x03,0x04,0x04,0x00,0x00,EOT}},
		{0x8, "Fan speed control",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x00,EOT}},
		{EOT}}},
	{0xe4, "PC87364", {
		{EOT}}},
	{0xe5, "PC87365", {	/* SRID[7..0] == chip revision */
		{EOT}}},
	{0xe8, "PC87363", {
		{EOT}}},
	{0xe9, "PC87366", {
		{EOT}}},

	/* SID[7..0]: family, SRID[7..5]: ID, SRID[4..0]: rev. */
	{0xea, "PC8739x", {
		{EOT}}},
	{0xec, "PC87591x", {
		/* SRID[7..5]: 000=PC87591E, 001=PC87591S, 100=PC87591L */
		{EOT}}},
	{0xee, "PC8741x", {
		/* SRID[7..5] is marked as "not applicable for the PC8741x". */
		{EOT}}},
	{0xf0, "PC87372", {
		{EOT}}},
	{0xf1, "PC8374L", {
		{EOT}}},
	{0xf2, "PC87427", {
		/* SRID[7..5] is marked as "not applicable for the PC87427". */
		{EOT}}},
	{0xf3, "PC87373", {
		{EOT}}},
	{EOT}
};

static void dump_readable_pc8374l(uint16_t port)
{
	if (!dump_readable)
		return;

	printf("Human-readable register dump:\n");

	printf("Enables: 21=%02x, 22=%02x, 23=%02x, 24=%02x, 26=%02x\n",
	       regval(port, 0x21), regval(port, 0x22), regval(port, 0x23),
	       regval(port, 0x24), regval(port, 0x26));
	printf("SMBUS at %02x\n", regval(port, 0x2a));

	/* Check COM1. This is all we care about at present. */
	printf("COM 1 is globally %s\n",
	       regval(port, 0x26) & 8 ? "disabled" : "enabled");

	/* Select COM1. */
	regwrite(port, 0x07, 0x03);
	printf("COM 1 is locally %s\n",
	       regval(port, 0x30) & 1 ? "enabled" : "disabled");
	printf
	    ("COM1 60=%02x, 61=%02x, 70=%02x, 71=%02x, 74=%02x, 75=%02x, f0=%02x\n",
	     regval(port, 0x60), regval(port, 0x61), regval(port, 0x70),
	     regval(port, 0x71), regval(port, 0x74), regval(port, 0x75),
	     regval(port, 0xf0));

	/* Select GPIO. */
	regwrite(port, 0x07, 0x07);
	printf("GPIO is %s\n", regval(port, 0x30) & 1 ? "enabled" : "disabled");
	printf
	    ("GPIO 60=%02x, 61=%02x, 70=%02x, 71=%02x, 74=%02x, 75=%02x, f0=%02x\n",
	     regval(port, 0x60), regval(port, 0x61), regval(port, 0x70),
	     regval(port, 0x71), regval(port, 0x74), regval(port, 0x75),
	     regval(port, 0xf0));
}

void probe_idregs_nsc(uint16_t port)
{
	uint8_t id, rev;

	probing_for("NSC", "", port);

	outb(CHIP_ID_REG, port);
	if (inb(port) != CHIP_ID_REG) {
		if (verbose)
			printf(NOTFOUND "port=0x%02x, port+1=0x%02x\n",
			       inb(port), inb(port + 1));
		return;
	}
	id = inb(port + 1);

	outb(CHIP_REV_REG, port);
	if (inb(port) != CHIP_REV_REG) {
		printf("Warning: Can't get chip revision. Setting to 0xff.\n");
		rev = 0xff;
	} else {
		rev = inb(port + 1);
	}

	if (superio_unknown(reg_table, id)) {
		if (verbose)
			printf(NOTFOUND "sid=0x%02x, srid=0x%02x\n", id, rev);
		return;
	}

	printf("Found NSC %s (sid=0x%02x, srid=0x%02x) at 0x%x\n",
	       get_superio_name(reg_table, id), id, rev, port);
	chip_found = 1;

	dump_superio("NSC", reg_table, port, id);
	if (id == 0xf1)
		dump_readable_pc8374l(port);
}

