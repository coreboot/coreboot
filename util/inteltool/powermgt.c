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

#include <stdio.h>
#include <sys/io.h>

#include "inteltool.h"

int print_pmbase(struct pci_dev *sb)
{
	int i, size = 0x80;
	uint16_t pmbase;

	printf("\n============= PMBASE ============\n\n");

	switch (sb->device_id) {
	case PCI_DEVICE_ID_INTEL_ICH7:
	case PCI_DEVICE_ID_INTEL_ICH7M:
	case PCI_DEVICE_ID_INTEL_ICH7DH:
	case PCI_DEVICE_ID_INTEL_ICH7MDH:
		pmbase = pci_read_word(sb, 0x40) & 0xfffc;
		break;
	case 0x1234: // Dummy for non-existent functionality
		printf("This southbridge does not have PMBASE.\n");
		return 1;
	default:
		printf("Error: Dumping PMBASE on this southbridge is not (yet) supported.\n");
		return 1;
	}

	printf("PMBASE = 0x%04x (IO)\n\n", pmbase);

	for (i = 0; i < size; i += 4) {
		printf("pmbase+0x%04x: 0x%08x\n", i, inl(pmbase + i));
	}

	return 0;
}

