/*
 * acpi_reset.c: Reboot your LinuxBIOS system with ACPI software watchdo
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
 *
 * $Id$
 */

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/io.h>
#include <unistd.h>
#include <stdio.h>

unsigned short acpi_base;

void
waitsmbus()
{
	unsigned short port = acpi_base;
	unsigned char val;

	//printf("waitsmb ..\n");

	for (val = inb(port); (val & 8) == 0; val = inb(port))
		;
	//printf("past first test\n");
}

void
setsmbus(unsigned char index, unsigned char value)
{
	unsigned short port = acpi_base + index;

	//printf("setsmbus: index 0x%02x, value 0x%02x\n", 
	//	index, value);

	outb(value, port);
}

unsigned char
getsmbus(unsigned char index)
{
	unsigned short port = acpi_base + index;
	unsigned char value;

	value = inb(port);

	//printf("getsmbus: index 0x%02x, value 0x%02x\n", 
	//	index, value);

	return value;
}

unsigned char
read_spd(unsigned char slot, unsigned char index)
{
	unsigned char value;

	setsmbus(0x03, 0x20);

	setsmbus(0x04, 0xA1 + (slot << 1));

	setsmbus(0x05, index);

	setsmbus(0x03, 0x12);

	waitsmbus();

	value = getsmbus(0x08);

	setsmbus(0x00, 0xFF);

	return value;
}

main()
{
	unsigned char b;
	unsigned short w;


	/* get io privilege access PCI configuration space */
	if (iopl(3) != 0) {
		perror("Can not set io priviliage");
		exit(1);
	}

	/* Enable ACPI by set B7 on Reg 0x40, LPC */
	outl(0x80000840, 0x0cf8);
	b = inb(0x0cfc) | 0x80;
	outb(b, 0xcfc);

	/* get the ACPI base address for register 0x74,0x75 of LPC */
	outl(0x80000874, 0x0cf8);
	w = inw(0x0cfc);
	acpi_base = w + 0x80;

	printf("Number of bytes used by module manufacturer 0x%02x\n",
	       read_spd(0x00, 0x00));

	printf("Memory Type 0x%02x\n",
	       read_spd(0x00, 0x02));

	printf("Number of Row Address bits 0x%02x\n",
	       read_spd(0x00, 0x03));

	printf("Number of Column Address bits 0x%02x\n",
	       read_spd(0x00, 0x04));

	printf("Number of Sides 0x%02x\n",
	       read_spd(0x00, 0x05));

	printf("Number of Banks 0x%02x\n",
	       read_spd(0x00, 0x11));
}
