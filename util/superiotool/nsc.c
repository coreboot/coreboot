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

/* SID[7..0]: chip family. SRID[7..5]: chip ID, SRID[4..0]: chip rev. */
const static struct superio_registers reg_table[] = {
	{0xf1, "PC8374L", {
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

