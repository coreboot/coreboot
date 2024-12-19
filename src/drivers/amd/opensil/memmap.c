/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <cbmem.h>
#include <cpu/amd/mtrr.h>
#include <string.h>
#include <vendorcode/amd/opensil/opensil.h>

#include "opensil.h"

/*
 * This structure definition must align exactly with the MEMORY_HOLE_TYPES structure
 * defined in openSIL to ensure accurate casting.
 */
typedef struct {
	uint64_t base;
	uint64_t size;
	uint32_t type;
	uint32_t reserved;
} HOLE_INFO;

/* This assumes holes are allocated */
void amd_opensil_add_memmap(struct device *dev, unsigned long *idx)
{
	uint64_t top_of_mem = 0;
	uint32_t n_holes = 0;
	void *hole_info = NULL;

	/* Account for UMA and TSEG */
	const uint32_t mem_usable = cbmem_top();
	const uint32_t top_mem = ALIGN_DOWN(get_top_of_mem_below_4gb(), 1 * MiB);
	if (mem_usable != top_mem)
		reserved_ram_from_to(dev, (*idx)++, mem_usable, top_mem);

	/* Holes in upper DRAM */
	/* This assumes all the holes in upper DRAM are continuous */
	opensil_get_hole_info(&n_holes, &top_of_mem, &hole_info);
	if (hole_info == NULL)
		return;

	/* Check if we're done */
	if (top_of_mem <= 4ULL * GiB)
		return;

	HOLE_INFO *holes = (HOLE_INFO *)hole_info;

	uint64_t lowest_upper_hole_base = top_of_mem;
	uint64_t highest_upper_hole_end = 4ULL * GiB;
	for (size_t hole = 0; hole < n_holes; hole++) {
		if (!strcmp(opensil_get_hole_info_type(holes[hole].type), "MMIO"))
			continue;
		if (holes[hole].base < 4ULL * GiB)
			continue;
		lowest_upper_hole_base = MIN(lowest_upper_hole_base, holes[hole].base);
		highest_upper_hole_end = MAX(highest_upper_hole_end, holes[hole].base + holes[hole].size);
		if (!strcmp(opensil_get_hole_info_type(holes[hole].type), "UMA"))
			mmio_range(dev, (*idx)++, holes[hole].base, holes[hole].size);
		else
			reserved_ram_range(dev, (*idx)++, holes[hole].base, holes[hole].size);
	}

	ram_from_to(dev, (*idx)++, 4ULL * GiB, lowest_upper_hole_base);

	if (top_of_mem > highest_upper_hole_end)
		ram_from_to(dev, (*idx)++, highest_upper_hole_end, top_of_mem);
}

static void print_memory_holes(void *unused)
{
	uint64_t top_of_mem = 0;
	uint32_t n_holes = 0;
	void *hole_info = NULL;

	opensil_get_hole_info(&n_holes, &top_of_mem, &hole_info);
	if (hole_info == NULL)
		return;

	HOLE_INFO *holes = (HOLE_INFO *)hole_info;

	printk(BIOS_DEBUG, "APOB: top of memory 0x%016llx\n", top_of_mem);
	printk(BIOS_DEBUG, "The following holes are reported in APOB\n");
	for (size_t hole = 0; hole < n_holes; hole++) {
		printk(BIOS_DEBUG, "  Base: 0x%016llx, Size: 0x%016llx, Type: %02d:%s\n",
			holes[hole].base, holes[hole].size, holes[hole].type,
			opensil_get_hole_info_type(holes[hole].type));
	}
}

BOOT_STATE_INIT_ENTRY(BS_DEV_RESOURCES, BS_ON_ENTRY, print_memory_holes, NULL);
