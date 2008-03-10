/*
 * dump gpio on intel ICH series southbridges
 *
 * Copyright (C) 2008 by coresystems GmbH 
 * written by Stefan Reinauer <stepan@coresystems.de> 
 * Copyright (C) 2008 Joseph Smith <joe@smittys.pointclark.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>

#include <sys/io.h>
#include <pci/pci.h>

int map_gpio(uint16_t gpio)
{
	int i;
	unsigned long size=0x40;

	for (i=0; i<size; i+=4) {
		printf("gpiobase+0x%04x: 0x%08x\n", i, inl(gpio+i));
	}

	return 0;
}

int main(int argc, char *argv[])
{
	struct pci_access *pacc;
	struct pci_dev *sb;
	uint16_t gpiobadd;
	uint16_t device;

	if (iopl(3)) { 
		perror("You need to be root.\n"); 
		exit(1);
	}

	pacc = pci_alloc();
	pci_init(pacc);
	pci_scan_bus(pacc);

	sb = pci_get_dev(pacc, 0, 0, 0x1f, 0);
	if (!sb) {
		printf("No southbridge found.\n");
		exit(1);
	}

	if (pci_read_word(sb, 0) != 0x8086) {
		printf("Not an Intel southbridge.\n");
		exit(1);
	}

	printf("Intel Southbridge: %04x:%04x\n", 
		pci_read_word(sb, 0), pci_read_word(sb, 2));

	device = pci_read_word(sb, 2);

	if (device < 0x2640) {
		gpiobadd = pci_read_word(sb, 0x58) & 0xfffc; 
	} else if (device >= 0x2640) {
		gpiobadd = pci_read_word(sb, 0x48) & 0xfffc;
	}
	printf("GPIOBASE = 0x%04x\n\n", gpiobadd);

	map_gpio(gpiobadd);


	return 0;
}
