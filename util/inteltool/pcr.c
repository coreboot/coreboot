/*
 * inteltool - dump all registers on an Intel CPU + chipset based system.
 *
 * Copyright (C) 2017 secunet Security Networks AG
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
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <assert.h>
#include "pcr.h"

const uint8_t *sbbar = NULL;

uint32_t read_pcr32(const uint8_t port, const uint16_t offset)
{
	assert(sbbar);
	return *(const uint32_t *)(sbbar + (port << 16) + offset);
}

static void print_pcr_port(const uint8_t port)
{
	size_t i = 0;
	uint32_t last_reg = 0;
	bool last_printed = true;

	printf("PCR port offset: 0x%06zx\n\n", (size_t)port << 16);

	for (i = 0; i < PCR_PORT_SIZE; i += 4) {
		const uint32_t reg = read_pcr32(port, i);
		const bool rep = i && last_reg == reg;
		if (!rep) {
			if (!last_printed)
				printf("*\n");
			printf("0x%04zx: 0x%08"PRIx32"\n", i, reg);
		}

		last_reg = reg;
		last_printed = !rep;
	}
	if (!last_printed)
		printf("*\n");
}

void print_pcr_ports(struct pci_dev *const sb,
		     const uint8_t *const ports, const size_t count)
{
	size_t i;

	pcr_init(sb);

	for (i = 0; i < count; ++i) {
		printf("\n========== PCR 0x%02x ==========\n\n", ports[i]);
		print_pcr_port(ports[i]);
	}
}

void pcr_init(struct pci_dev *const sb)
{
	bool error_exit = false;
	bool p2sb_revealed = false;

	if (sbbar)
		return;

	struct pci_dev *const p2sb = pci_get_dev(sb->access, 0, 0, 0x1f, 1);

	if (!p2sb) {
		perror("Can't allocate device node for P2SB.");
		exit(1);
	}

	/* do not fill bases here, libpci refuses to refill later */
	pci_fill_info(p2sb, PCI_FILL_IDENT);
	if (p2sb->vendor_id == 0xffff && p2sb->device_id == 0xffff) {
		printf("Trying to reveal Primary to Sideband Bridge "
		       "(P2SB),\nlet's hope the OS doesn't mind... ");
		/* Do not use pci_write_long(). Surrounding
		   bytes 0xe0 must be maintained. */
		pci_write_byte(p2sb, 0xe0 + 1, 0);

		pci_fill_info(p2sb, PCI_FILL_IDENT | PCI_FILL_RESCAN);
		if (p2sb->vendor_id != 0xffff ||
		    p2sb->device_id != 0xffff) {
			printf("done.\n");
			p2sb_revealed = true;
		} else {
			printf("failed.\n");
			exit(1);
		}
	}
	pci_fill_info(p2sb, PCI_FILL_BASES | PCI_FILL_CLASS);

	const pciaddr_t sbbar_phys = p2sb->base_addr[0] & ~0xfULL;
	printf("SBREG_BAR = 0x%08"PRIx64" (MEM)\n\n", (uint64_t)sbbar_phys);
	sbbar = map_physical(sbbar_phys, SBBAR_SIZE);
	if (sbbar == NULL) {
		perror("Error mapping SBREG_BAR");
		error_exit = true;
	}

	if (p2sb_revealed) {
		printf("Hiding Primary to Sideband Bridge (P2SB).\n");
		pci_write_byte(p2sb, 0xe0 + 1, 1);
	}
	pci_free_dev(p2sb);

	if (error_exit)
		exit(1);
}

void pcr_cleanup(void)
{
	if (sbbar)
		unmap_physical((void *)sbbar, SBBAR_SIZE);
}
