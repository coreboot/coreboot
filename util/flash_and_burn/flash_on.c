/*
 * flash_rom.c: Turnning on Flash Write Enable for SiS 630/950 M/Bs,
 *              use this program before loading DoC drivers.
 *
 *
 * Copyright 2000 Silicon Integrated System Corporation
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * Reference:
 *	1. SiS 630 Specification
 *	2. SiS 950 Specification
 *
 * $Id$
 */

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/io.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	char b;

	/* get io privilege access PCI configuration space */
	if (iopl(3) != 0) {
		perror("Can not set io priviliage");
		exit(1);
	}

	/* Enable 0xFFF8000~0xFFFF0000 decoding on SiS 540/630 */
	outl(0x80000840, 0x0cf8);
	b = inb(0x0cfc) | 0x0b;
	outb(b, 0xcfc);
	/* Flash write enable on SiS 540/630 */
	outl(0x80000845, 0x0cf8);
	b = inb(0x0cfd) | 0x40;
	outb(b, 0xcfd);

	/* The same thing on SiS 950 SuperIO side */
	outb(0x87, 0x2e);
	outb(0x01, 0x2e);
	outb(0x55, 0x2e);
	outb(0x55, 0x2e);

	if (inb(0x2f) != 0x87) {
		printf("Can not access SiS 950\n");
		return -1;
	}

	outb(0x24, 0x2e);
	b = inb(0x2f) | 0xfc;
	outb(0x24, 0x2e);
	outb(b, 0x2f);

	outb(0x02, 0x2e);
	outb(0x02, 0x2f);

	return (0);
}
