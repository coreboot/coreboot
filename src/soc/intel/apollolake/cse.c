/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Google, Inc.
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

#include <arch/io.h>
#include <bootstate.h>
#include <console/console.h>
#include <soc/pci_devs.h>
#include <stdint.h>

#define PCI_ME_HFSTS1	0x40
#define PCI_ME_HFSTS2	0x48
#define PCI_ME_HFSTS3	0x60
#define PCI_ME_HFSTS4	0x64
#define PCI_ME_HFSTS5	0x68
#define PCI_ME_HFSTS6	0x6c

static uint32_t dump_status(int index, int reg_addr)
{
	uint32_t reg = pci_read_config32(CSE_DEV, reg_addr);

	printk(BIOS_DEBUG, "CSE FWSTS%d: 0x%08x\n", index, reg);

	return reg;
}

static void dump_cse_state(void *unused)
{
	uint32_t fwsts1;

	fwsts1 = dump_status(1, PCI_ME_HFSTS1);
	dump_status(2, PCI_ME_HFSTS2);
	dump_status(3, PCI_ME_HFSTS3);
	dump_status(4, PCI_ME_HFSTS4);
	dump_status(5, PCI_ME_HFSTS5);
	dump_status(6, PCI_ME_HFSTS6);

	/* Minimal decoding is done here in order to call out most important
	   pieces. Manufacturing mode needs to be locked down prior to shipping
	   the product so it's called out explicitly. */
	printk(BIOS_DEBUG, "ME: Manufacturing Mode      : %s\n",
		(fwsts1 & (1 << 0x4)) ? "YES" : "NO");
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, dump_cse_state, NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT, dump_cse_state, NULL);
