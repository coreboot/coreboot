/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
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
#include "ck804.h"

static void ck804_enable_rom(void)
{
	unsigned char byte;
	pci_devfn_t addr;

	/* Enable 4MB ROM access at 0xFFC00000 - 0xFFFFFFFF. */
	/* Locate the ck804 LPC. */
	addr = PCI_DEV(0, (CK804_DEVN_BASE + 1), 0);

	/* Set the 4MB enable bit. */
	byte = pci_read_config8(addr, 0x88);
	byte |= 0x80;
	pci_write_config8(addr, 0x88, byte);
}

static void bootblock_southbridge_init(void)
{
	ck804_enable_rom();
}
