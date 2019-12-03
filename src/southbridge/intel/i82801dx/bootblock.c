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

#include <arch/bootblock.h>
#include <device/pci_ops.h>

void bootblock_early_southbridge_init(void)
{
	/* Set FWH IDs for 2 MB flash part. */
	if (CONFIG_ROM_SIZE == 0x200000)
		pci_write_config32(PCI_DEV(0, 0x1f, 0), 0xe8, 0x00001111);
}
