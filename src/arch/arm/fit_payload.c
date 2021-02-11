/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <bootmem.h>
#include <program_loading.h>
#include <fit.h>
#include <symbols.h>

/**
 * Place the region in free memory range.
 */
static bool fit_place_mem(const struct range_entry *r, void *arg)
{
	struct region *region = arg;
	resource_t start;

	if (range_entry_tag(r) != BM_MEM_RAM)
		return true;

	/* Linux 4.15 doesn't like 4KiB alignment. Align to 1 MiB for now. */
	start = ALIGN_UP(MAX(region->offset, range_entry_base(r)), 1 * MiB);

	if (start + region->size < range_entry_end(r)) {
		region->offset = (size_t)start;
		return false;
	}

	return true;
}

bool fit_payload_arch(struct prog *payload, struct fit_config_node *config,
		      struct region *kernel,
		      struct region *fdt,
		      struct region *initrd)
{
	void *arg = NULL;

	/**
	 * The kernel ARM documentation recommends loading the kernel above 32MiB
	 * in order to avoid the need to need to relocate prior to decompression.
	 */
	kernel->offset = (uintptr_t)_dram + 32 * MiB;

	/**
	 * The code assumes that bootmem_walk provides a sorted list of memory
	 * regions, starting from the lowest address.
	 * The order of the calls here doesn't matter, as the placement is
	 * enforced in the called functions.
	 * For details check code on top.
	 */
	if (!bootmem_walk(fit_place_mem, kernel))
		return false;

	/* Mark as reserved for future allocations. */
	bootmem_add_range(kernel->offset, kernel->size, BM_MEM_PAYLOAD);

	/**
	 * To ensure the fdt is not overwritten by the kernel decompressor, place
	 * the fdt above the 128 MB from the start of RAM, as recommended by the
	 * kernel documentation.
	 */
	fdt->offset = (uintptr_t)_dram + 128 * MiB;

	if (!bootmem_walk(fit_place_mem, fdt))
		return false;

	/* Mark as reserved for future allocations. */
	bootmem_add_range(fdt->offset, fdt->size, BM_MEM_PAYLOAD);

	/* Place INITRD */
	if (config->ramdisk) {
		initrd->offset = fdt->offset + fdt->size;

		if (!bootmem_walk(fit_place_mem, initrd))
			return false;

		/* Mark as reserved for future allocations. */
		bootmem_add_range(initrd->offset, initrd->size, BM_MEM_PAYLOAD);
	}

	/* Kernel expects FDT as argument */
	arg = (void *)fdt->offset;

	prog_set_entry(payload, (void *)kernel->offset, arg);

	bootmem_dump_ranges();

	return true;
}
