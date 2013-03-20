/*
 * support for Multiboot payloads
 *
 * Copyright (C) 2008 Robert Millan
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <cpu/x86/multiboot.h>
#include <string.h>
#include <device/resource.h>
#include <console/console.h>
#include <boot/coreboot_tables.h>

struct multiboot_info *mbi = NULL;

unsigned long write_multiboot_info(unsigned long rom_table_end)
{
	static struct multiboot_mmap_entry *mb_mem;
	struct lb_memory* coreboot_table;
	int entries;
	int i;

	mbi = (struct multiboot_info *)rom_table_end;

	memset(mbi, 0, sizeof(*mbi));
	rom_table_end += sizeof(*mbi);

	mbi->mmap_addr = (u32) rom_table_end;
	mb_mem = (struct multiboot_mmap_entry *)rom_table_end;

	/* copy regions from coreboot tables */
	coreboot_table = get_lb_mem();
	entries = (coreboot_table->size - sizeof(*coreboot_table))/sizeof(coreboot_table->map[0]);

	if (coreboot_table == NULL || entries < 1) {
	    printk(BIOS_INFO, "%s: Cannot find coreboot table.\n", __func__);
	    return (unsigned long) mb_mem;
	}

	for (i = 0; i < entries; i++) {
	  uint64_t entry_start = unpack_lb64(coreboot_table->map[i].start);
	  uint64_t entry_size = unpack_lb64(coreboot_table->map[i].size);
	  mb_mem->addr = entry_start;
	  mb_mem->len = entry_size;
	  switch (coreboot_table->map[i].type) {
	    case LB_MEM_RAM:
	      mb_mem->type = MULTIBOOT_MEMORY_AVAILABLE;
	      break;
	    default: // anything other than usable RAM
	      mb_mem->type = MULTIBOOT_MEMORY_RESERVED;
	      break;
	  }
	  mb_mem->size = sizeof(*mb_mem) - sizeof(mb_mem->size);
	  mb_mem++;
	}

	mbi->mmap_length = ((u32) mb_mem) - mbi->mmap_addr;
	mbi->flags |= MB_INFO_MEM_MAP;

	printk(BIOS_INFO, "Multiboot Information structure has been written.\n");

	return (unsigned long)mb_mem;
}
