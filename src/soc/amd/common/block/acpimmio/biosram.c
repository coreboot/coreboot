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

#include <cbmem.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/biosram.h>

void *get_ap_entry_ptr(void)
{
	return (void *)biosram_read32(BIOSRAM_AP_ENTRY);
}

void set_ap_entry_ptr(void *entry)
{
	biosram_write32(BIOSRAM_AP_ENTRY, (uintptr_t)entry);
}


void backup_top_of_low_cacheable(uintptr_t ramtop)
{
	biosram_write32(BIOSRAM_CBMEM_TOP, ramtop);
}

uintptr_t restore_top_of_low_cacheable(void)
{
	return biosram_read32(BIOSRAM_CBMEM_TOP);
}

void save_uma_size(uint32_t size)
{
	biosram_write32(BIOSRAM_UMA_SIZE, size);
}

void save_uma_base(uint64_t base)
{
	biosram_write32(BIOSRAM_UMA_BASE, (uint32_t) base);
	biosram_write32(BIOSRAM_UMA_BASE + 4, (uint32_t) (base >> 32));
}

uint32_t get_uma_size(void)
{
	return biosram_read32(BIOSRAM_UMA_SIZE);
}

uint64_t get_uma_base(void)
{
	uint64_t base;
	base = biosram_read32(BIOSRAM_UMA_BASE);
	base |= ((uint64_t)(biosram_read32(BIOSRAM_UMA_BASE + 4)) << 32);
	return base;
}
