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

#include <multiboot.h>
#include <string.h>
#include <device/resource.h>
#include <console.h>

static struct multiboot_mmap_entry *mb_mem;

static void build_mb_mem_range(void *gp, struct device *dev, struct resource *res)
{
	mb_mem->addr = res->base;
	mb_mem->len = res->size;
	mb_mem->type = 1;
	mb_mem->size = sizeof(*mb_mem) - sizeof(mb_mem->size);
	mb_mem++;
}

unsigned long write_multiboot_info(
	unsigned long low_table_start, unsigned long low_table_end,
	unsigned long rom_table_start, unsigned long rom_table_end)
{
	struct multiboot_info *mbi = rom_table_end;
	memset(mbi, 0, sizeof(*mbi));
	rom_table_end += sizeof(*mbi);

	mbi->mmap_addr = (u32) rom_table_end;
	mb_mem = rom_table_end;

	/* free regions */
	search_global_resources( IORESOURCE_MEM | IORESOURCE_CACHEABLE,
		IORESOURCE_MEM | IORESOURCE_CACHEABLE, build_mb_mem_range, NULL);

	/* reserved regions */
	mb_mem->addr = low_table_start;
	mb_mem->len = low_table_end - low_table_start;
	mb_mem->type = 2;
	mb_mem->size = sizeof(*mb_mem) - sizeof(mb_mem->size);
	mb_mem++;
	mb_mem->addr = rom_table_start;
	mb_mem->len = rom_table_end - rom_table_start;
	mb_mem->type = 2;
	mb_mem->size = sizeof(*mb_mem) - sizeof(mb_mem->size);
	mb_mem++;

	mbi->mmap_length = ((u32) mb_mem) - mbi->mmap_addr;
	mbi->flags |= MB_INFO_MEM_MAP;

	printk(BIOS_INFO, "Multiboot Information structure has been written.\n");

	return mb_mem;
}
