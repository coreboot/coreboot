/*
 * inteltool - dump all registers on an Intel CPU + chipset based system.
 *
 * Copyright (C) 2008 by coresystems GmbH 
 *  written by Stefan Reinauer <stepan@coresystems.de> 
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>
#include <sys/mman.h>
#include <sys/io.h>
#include <pci/pci.h>

#include "inteltool.h"

int print_rcba(struct pci_dev *sb)
{
	int i, size = 0x4000;
	volatile uint8_t *rcba;
	uint32_t rcba_phys;

	printf("\n============= RCBA ==============\n\n");

	switch (sb->device_id) {
	case PCI_DEVICE_ID_INTEL_ICH7:
	case PCI_DEVICE_ID_INTEL_ICH7M:
	case PCI_DEVICE_ID_INTEL_ICH7DH:
	case PCI_DEVICE_ID_INTEL_ICH7MDH:
		rcba_phys = pci_read_long(sb, 0xf0) & 0xfffffffe;
		break;
	case PCI_DEVICE_ID_INTEL_ICH:
	case PCI_DEVICE_ID_INTEL_ICH0:
	case PCI_DEVICE_ID_INTEL_ICH4:
	case PCI_DEVICE_ID_INTEL_ICH4M:
		printf("This southbridge does not have RCBA.\n");
		return 1;
	default:
		printf("Error: Dumping RCBA on this southbridge is not (yet) supported.\n");
		return 1;
	}

	rcba = mmap(0, size, PROT_WRITE | PROT_READ, MAP_SHARED,
		    fd_mem, (off_t) rcba_phys);
	
	if (rcba == MAP_FAILED) {
		perror("Error mapping RCBA");
		exit(1);
	}

	printf("RCBA = 0x%08x (MEM)\n\n", rcba_phys);

	for (i = 0; i < size; i += 4) {
		if (*(uint32_t *)(rcba + i))
			printf("0x%04x: 0x%08x\n", i, *(uint32_t *)(rcba + i));
	}

	munmap((void *)rcba, size);
	return 0;
}

