/*
 * Copyright (C) 2014  Damien Zammit <damien@zamaudio.com>
 * Copyright (C) 2017  Patrick Rudolph <siro@das-labor.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdio.h>
#include <inttypes.h>

#include "intelmetool.h"
#include "mmap.h"
#include "rcba.h"

static const int size = 0x4000;

/* Returns the physical RCBA base address or zero on error. */
u32 get_rcba_phys(void)
{
	struct pci_access *pacc;
	struct pci_dev *sb;
	uint32_t rcba_phys;

	pacc = pci_alloc();
	pacc->method = PCI_ACCESS_I386_TYPE1;

	pci_init(pacc);
	pci_scan_bus(pacc);

	sb = pci_get_dev(pacc, 0, 0, 0x1f, 0);
	if (!sb) {
		printf("Uh oh, southbridge not on BDF(0:31:0), please report "
		       "this error, exiting.\n");
		pci_cleanup(pacc);
		return 0;
	}
	pci_fill_info(sb, PCI_FILL_IDENT | PCI_FILL_BASES | PCI_FILL_SIZES |
			  PCI_FILL_CLASS);

	rcba_phys = pci_read_long(sb, 0xf0) & 0xfffffffe;

	pci_free_dev(sb);
	pci_cleanup(pacc);

	return rcba_phys;
}

/*
 * Writes 'val' to RCBA register at address 'addr'.
 * Returns 1 on error and 0 on success.
 */
int write_rcba32(uint32_t addr, uint32_t val)
{
	volatile uint8_t *rcba;
	const uint32_t rcba_phys = get_rcba_phys();

	if (!rcba_phys) {
		printf("Could not get RCBA address\n");
		return 1;
	}

	rcba = map_physical((off_t)rcba_phys, size);
	if (rcba == NULL) {
		printf("Could not map RCBA\n"
		       "Do you have cmdline argument 'iomem=relaxed' set ?\n");
		return 1;
	}
	*(uint32_t *)(rcba + addr) = val;

	munmap((void *)rcba, size);
	return 0;
}

/*
 * Reads RCBA register at address 'addr' and stores it in 'val'.
 * Returns 1 on error and 0 on success.
 */
int read_rcba32(uint32_t addr, uint32_t *val)
{
	volatile uint8_t *rcba;
	const uint32_t rcba_phys = get_rcba_phys();

	if (!rcba_phys) {
		printf("Could not get RCBA address\n");
		return 1;
	}

	rcba = map_physical((off_t)rcba_phys, size);
	if (rcba == NULL) {
		printf("Could not map RCBA\n"
		       "Do you have cmdline argument 'iomem=relaxed' set ?\n");
		return 1;
	}

	*val = *(uint32_t *)(rcba + addr);

	munmap((void *)rcba, size);
	return 0;
}
