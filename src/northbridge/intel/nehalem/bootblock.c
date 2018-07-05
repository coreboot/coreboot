/*
 * This file is part of the coreboot project.
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

static void bootblock_northbridge_init(void)
{
	pci_io_write_config32(PCI_DEV(0xff, 0x00, 1), 0x50, DEFAULT_PCIEXBAR | 1);
	pci_io_write_config32(PCI_DEV(0xff, 0x00, 1), 0x54, 0);
}
