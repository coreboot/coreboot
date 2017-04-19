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

#define __SIMPLE_DEVICE__

#include <stdint.h>
#include <arch/io.h>
#include <cbmem.h>

#define CBMEM_TOP_SCRATCHPAD 0x78

void backup_top_of_low_cacheable(uintptr_t ramtop)
{
	uint16_t top_cache = ramtop >> 16;
	pci_write_config16(PCI_DEV(0,0,0), CBMEM_TOP_SCRATCHPAD, top_cache);
}

uintptr_t restore_top_of_low_cacheable(void)
{
	uint16_t top_cache;
	top_cache = pci_read_config16(PCI_DEV(0,0,0), CBMEM_TOP_SCRATCHPAD);
	return (top_cache << 16);
}
