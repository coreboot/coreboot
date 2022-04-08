/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <bootmem.h>
#include <cbmem.h>
#include <device/resource.h>
#include <stdlib.h>
#include <symbols.h>
#include <assert.h>

static int initialized;
static int table_written;
static struct memranges bootmem;
static struct memranges bootmem_os;

static int bootmem_is_initialized(void)
{
	return initialized;
}

static int bootmem_memory_table_written(void)
{
	return table_written;
}

/* Platform hook to add bootmem areas the platform / board controls. */
void __attribute__((weak)) bootmem_platform_add_ranges(void)
{
}

/* Convert bootmem tag to LB_MEM tag */
static uint32_t bootmem_to_lb_tag(const enum bootmem_type tag)
{
	switch (tag) {
	case BM_MEM_RAM:
		return LB_MEM_RAM;
	case BM_MEM_RESERVED:
		return LB_MEM_RESERVED;
	case BM_MEM_ACPI:
		return LB_MEM_ACPI;
	case BM_MEM_NVS:
		return LB_MEM_NVS;
	case BM_MEM_UNUSABLE:
		return LB_MEM_UNUSABLE;
	case BM_MEM_VENDOR_RSVD:
		return LB_MEM_VENDOR_RSVD;
	case BM_MEM_OPENSBI:
		return LB_MEM_RESERVED;
	case BM_MEM_BL31:
		return LB_MEM_RESERVED;
	case BM_MEM_TABLE:
		return LB_MEM_TABLE;
	default:
		printk(BIOS_ERR, "Unsupported tag %u\n", tag);
		return LB_MEM_RESERVED;
	}
}

static void bootmem_init(void)
{
	const unsigned long cacheable = IORESOURCE_CACHEABLE;
	const unsigned long reserved = IORESOURCE_RESERVE;
	struct memranges *bm = &bootmem;

	initialized = 1;

	/*
	 * Fill the memory map out. The order of operations is important in
	 * that each overlapping range will take over the next. Therefore,
	 * add cacheable resources as RAM then add the reserved resources.
	 */
	memranges_init(bm, cacheable, cacheable, BM_MEM_RAM);
	memranges_add_resources(bm, reserved, reserved, BM_MEM_RESERVED);
	memranges_clone(&bootmem_os, bm);

	/* Add memory used by CBMEM. */
	cbmem_add_bootmem();

	bootmem_add_range((uintptr_t)_stack, REGION_SIZE(stack),
			  BM_MEM_RAMSTAGE);
	bootmem_add_range((uintptr_t)_program, REGION_SIZE(program),
			  BM_MEM_RAMSTAGE);

	bootmem_arch_add_ranges();
	bootmem_platform_add_ranges();
}

void bootmem_add_range(uint64_t start, uint64_t size,
		       const enum bootmem_type tag)
{
	assert(tag > BM_MEM_FIRST && tag < BM_MEM_LAST);
	assert(bootmem_is_initialized());

	memranges_insert(&bootmem, start, size, tag);
	if (tag <= BM_MEM_OS_CUTOFF) {
		/* Can't change OS tables anymore after they are written out. */
		assert(!bootmem_memory_table_written());
		memranges_insert(&bootmem_os, start, size, tag);
	};
}

void bootmem_write_memory_table(struct lb_memory *mem)
{
	const struct range_entry *r;
	struct lb_memory_range *lb_r;

	lb_r = &mem->map[0];

	bootmem_init();
	bootmem_dump_ranges();

	memranges_each_entry(r, &bootmem_os) {
		lb_r->start = range_entry_base(r);
		lb_r->size = range_entry_size(r);
		lb_r->type = bootmem_to_lb_tag(range_entry_tag(r));

		lb_r++;
		mem->size += sizeof(struct lb_memory_range);
	}

	table_written = 1;
}

struct range_strings {
	enum bootmem_type tag;
	const char *str;
};

static const struct range_strings type_strings[] = {
	{ BM_MEM_RAM, "RAM" },
	{ BM_MEM_RESERVED, "RESERVED" },
	{ BM_MEM_ACPI, "ACPI" },
	{ BM_MEM_NVS, "NVS" },
	{ BM_MEM_UNUSABLE, "UNUSABLE" },
	{ BM_MEM_VENDOR_RSVD, "VENDOR RESERVED" },
	{ BM_MEM_BL31, "BL31" },
	{ BM_MEM_OPENSBI, "OPENSBI" },
	{ BM_MEM_TABLE, "CONFIGURATION TABLES" },
	{ BM_MEM_RAMSTAGE, "RAMSTAGE" },
	{ BM_MEM_PAYLOAD, "PAYLOAD" },
};

static const char *bootmem_range_string(const enum bootmem_type tag)
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

bool bootmem_walk_os_mem(range_action_t action, void *arg)
{
	const struct range_entry *r;

	assert(bootmem_is_initialized());

	memranges_each_entry(r, &bootmem_os) {
		if (!action(r, arg))
			return true;
	}

	return false;
}

bool bootmem_walk(range_action_t action, void *arg)
{
	const struct range_entry *r;

	assert(bootmem_is_initialized());

	memranges_each_entry(r, &bootmem) {
		if (!action(r, arg))
			return true;
	}

	return false;
}

int bootmem_region_targets_type(uint64_t start, uint64_t size,
				enum bootmem_type dest_type)
{
	const struct range_entry *r;
	uint64_t end = start + size;

	memranges_each_entry(r, &bootmem) {
		/* All further bootmem entries are beyond this range. */
		if (end <= range_entry_base(r))
			break;

		if (start >= range_entry_base(r) && end <= range_entry_end(r)) {
			if (range_entry_tag(r) == dest_type)
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

	if (!bootmem_is_initialized()) {
		printk(BIOS_ERR, "%s: lib uninitialized!\n", __func__);
		return NULL;
	}

	/* 4KiB alignment. */
	size = ALIGN(size, 4096);
	region = NULL;
	memranges_each_entry(r, &bootmem) {
		if (range_entry_base(r) >= max_addr)
			break;

		if (range_entry_size(r) < size)
			continue;

		if (range_entry_tag(r) != BM_MEM_RAM)
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
	end = range_entry_end(region);
	if (end > max_addr)
		end = max_addr;
	begin = end - size;

	/* Mark buffer as unusable for future buffer use. */
	bootmem_add_range(begin, size, BM_MEM_PAYLOAD);

	return (void *)(uintptr_t)begin;
}
