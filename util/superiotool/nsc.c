/*
 * This file is part of the superiotool project.
 *
 * Copyright (C) 2006 Ronald Minnich <rminnich@gmail.com>
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

/* Well, they really thought this through, eh? Family is 8 bits! */
static const char *familyid[] = {
	[0xf1] = "PC8374 (Winbond/NatSemi)"
};

static void dump_readable_ns8374(uint16_t port)
{
	if (!dump_readable)
		return;

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

void probe_idregs_simple(uint16_t port)
{
	uint16_t id;

	outb(0x20, port);
	if (inb(port) != 0x20) {
		no_superio_found("NSC", "", port);
		/* TODO: Exit config mode? */
		return;
	}
	id = inb(port + 1);

	printf("Super I/O found at 0x%02x: id = 0x%02x\n", port, id);
	if (id == 0xff)
		return;

	if (familyid[id])
		printf("%s\n", familyid[id]);
	else
		printf("<unknown>\n");

	switch (id) {
	case 0xf1:
		dump_readable_ns8374(port);
		break;
	default:
		printf("No dump for 0x%02x\n", id);
		break;
	}
}

