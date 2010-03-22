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

static struct multiboot_mmap_entry *mb_mem;
struct multiboot_info *mbi = NULL;

static struct {
	u64 addr;
	u64 len;
} reserved_mem[2];

static void build_mb_mem_range_nooverlap(u64 addr, u64 len)
{
	int i;
	for (i = 0; i < sizeof(reserved_mem) / sizeof(reserved_mem[0]); i++) {
		/* free region fully contained in reserved region, abort */
		if (addr >= reserved_mem[i].addr && addr + len <= reserved_mem[i].addr + reserved_mem[i].len)
			return;
		/* reserved region splits free region */
		if (addr < reserved_mem[i].addr && addr + len > reserved_mem[i].addr + reserved_mem[i].len) {
			build_mb_mem_range_nooverlap(addr, reserved_mem[i].addr - addr);
			build_mb_mem_range_nooverlap(reserved_mem[i].addr + reserved_mem[i].len, (addr + len) - (reserved_mem[i].addr + reserved_mem[i].len));
			return;
		}
		/* left overlap */
		if (addr < reserved_mem[i].addr + reserved_mem[i].len && addr + len > reserved_mem[i].addr + reserved_mem[i].len) {
			len += addr;
			addr = reserved_mem[i].addr + reserved_mem[i].len;
			len -= addr;
			/* len += addr - old_addr */
			continue;
		}
		/* right overlap */
		if (addr < reserved_mem[i].addr && addr + len > reserved_mem[i].addr) {
			len = reserved_mem[i].addr - addr;
			continue;
		}
		/* none of the above, just add it */
	}

	mb_mem->addr = addr;
	mb_mem->len = len;
	mb_mem->type = 1;
	mb_mem->size = sizeof(*mb_mem) - sizeof(mb_mem->size);
	mb_mem++;
}

static void build_mb_mem_range(void *gp, struct device *dev, struct resource *res)
{
	build_mb_mem_range_nooverlap(res->base, res->size);
}

#define ROUND(_r,_a) (((_r) + (((_a) - 1))) & ~((_a) - 1))

unsigned long write_multiboot_info(
	unsigned long low_table_start, unsigned long low_table_end,
	unsigned long rom_table_start, unsigned long rom_table_end)
{
	int i;

	mbi = (struct multiboot_info *)rom_table_end;

	memset(mbi, 0, sizeof(*mbi));
	rom_table_end += sizeof(*mbi);

	mbi->mmap_addr = (u32) rom_table_end;
	mb_mem = (struct multiboot_mmap_entry *)rom_table_end;

	/* FIXME This code is broken, it does not know about high memory
	 * tables, nor does it reserve the coreboot table area.
	 */
	/* reserved regions */
	reserved_mem[0].addr = low_table_start;
	reserved_mem[0].len = ROUND(low_table_end - low_table_start, 4096);
	reserved_mem[1].addr = rom_table_start;
	reserved_mem[1].len = ROUND(rom_table_end - rom_table_start, 4096);

	for (i = 0; i < sizeof(reserved_mem) / sizeof(reserved_mem[0]); i++) {
		mb_mem->addr = reserved_mem[i].addr;
		mb_mem->len = reserved_mem[i].len;
		mb_mem->type = 2;
		mb_mem->size = sizeof(*mb_mem) - sizeof(mb_mem->size);
		mb_mem++;
	}

	/* free regions */
	search_global_resources( IORESOURCE_MEM | IORESOURCE_CACHEABLE,
		IORESOURCE_MEM | IORESOURCE_CACHEABLE, build_mb_mem_range, NULL);

	mbi->mmap_length = ((u32) mb_mem) - mbi->mmap_addr;
	mbi->flags |= MB_INFO_MEM_MAP;

	printk(BIOS_INFO, "Multiboot Information structure has been written.\n");

	return (unsigned long)mb_mem;
}
