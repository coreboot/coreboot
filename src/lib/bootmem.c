/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003-2004 Eric Biederman
 * Copyright (C) 2005-2010 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <bootmem.h>
#include <cbmem.h>
#include <device/resource.h>
#include <stdlib.h>

static struct memranges bootmem;

void bootmem_init(void)
{
	const unsigned long cacheable = IORESOURCE_CACHEABLE;
	const unsigned long reserved = IORESOURCE_RESERVE;
	struct memranges *bm = &bootmem;

	/*
	 * Fill the memory map out. The order of operations is important in
	 * that each overlapping range will take over the next. Therefore,
	 * add cacheable resources as RAM then add the reserved resources.
	 */
	memranges_init(bm, cacheable, cacheable, LB_MEM_RAM);
	memranges_add_resources(bm, reserved, reserved, LB_MEM_RESERVED);

	/* Add memory used by CBMEM. */
	cbmem_add_bootmem();
}

void bootmem_add_range(uint64_t start, uint64_t size, uint32_t type)
{
	memranges_insert(&bootmem, start, size, type);
}

void bootmem_write_memory_table(struct lb_memory *mem)
{
	const struct range_entry *r;
	struct lb_memory_range *lb_r;

	lb_r = &mem->map[0];

	bootmem_dump_ranges();

	memranges_each_entry(r, &bootmem) {
		lb_r->start = pack_lb64(range_entry_base(r));
		lb_r->size = pack_lb64(range_entry_size(r));
		lb_r->type = range_entry_tag(r);

		lb_r++;
		mem->size += sizeof(struct lb_memory_range);
	}
}

struct range_strings {
	unsigned long tag;
	const char *str;
};

static const struct range_strings type_strings[] = {
	{ LB_MEM_RAM, "RAM" },
	{ LB_MEM_RESERVED, "RESERVED" },
	{ LB_MEM_ACPI, "ACPI" },
	{ LB_MEM_NVS, "NVS" },
	{ LB_MEM_UNUSABLE, "UNUSABLE" },
	{ LB_MEM_VENDOR_RSVD, "VENDOR RESERVED" },
	{ LB_MEM_TABLE, "CONFIGURATION TABLES" },
};

static const char *bootmem_range_string(unsigned long tag)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(type_strings); i++) {
		if (type_strings[i].tag == tag)
			return type_strings[i].str;
	}

	return "UNKNOWN!";
}

void bootmem_dump_ranges(void)
{
	int i;
	const struct range_entry *r;

	i = 0;
	memranges_each_entry(r, &bootmem) {
		printk(BIOS_DEBUG, "%2d. %016llx-%016llx: %s\n",
			i, range_entry_base(r), range_entry_end(r) - 1,
			bootmem_range_string(range_entry_tag(r)));
		i++;
	}
}

int bootmem_region_targets_usable_ram(uint64_t start, uint64_t size)
{
	const struct range_entry *r;
	uint64_t end = start + size;

	memranges_each_entry(r, &bootmem) {
		/* All further bootmem entries are beyond this range. */
		if (end <= range_entry_base(r))
			break;

		if (start >= range_entry_base(r) && end <= range_entry_end(r)) {
			if (range_entry_tag(r) == LB_MEM_RAM)
				return 1;
		}
	}
	return 0;
}

void *bootmem_allocate_buffer(size_t size)
{
	const struct range_entry *r;
	const struct range_entry *region;
	/* All allocated buffers fall below the 32-bit boundary. */
	const resource_t max_addr = 1ULL << 32;
	resource_t begin;
	resource_t end;

	/* 4KiB alignment. */
	size = ALIGN(size, 4096);
	region = NULL;
	memranges_each_entry(r, &bootmem) {
		if (range_entry_size(r) < size)
			continue;

		if (range_entry_tag(r) != LB_MEM_RAM)
			continue;

		if (range_entry_base(r) >= max_addr)
			continue;

		end = range_entry_end(r);
		if (end > max_addr)
			end = max_addr;

		if ((end - range_entry_base(r)) < size)
			continue;

		region = r;
	}

	if (region == NULL)
		return NULL;

	/* region now points to the highest usable region for the given size. */
	begin = range_entry_base(region);
	end = range_entry_end(region);
	if (end > max_addr)
		end = max_addr;
	begin = end - size;

	/* Mark buffer as unusuable for future buffer use. */
	bootmem_add_range(begin, size, LB_MEM_UNUSABLE);

	return (void *)(uintptr_t)begin;
}
