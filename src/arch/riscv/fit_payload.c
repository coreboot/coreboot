/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <commonlib/bsd/compression.h>
#include <console/console.h>
#include <bootmem.h>
#include <program_loading.h>
#include <fit.h>
#include <endian.h>

/* Implements a Berkeley Boot Loader (BBL) compatible payload loading */

#define MAX_KERNEL_SIZE (64*MiB)

#if CONFIG(ARCH_RISCV_RV32)
#define SECTION_ALIGN (4 * MiB)
#endif
#if CONFIG(ARCH_RISCV_RV64)
#define SECTION_ALIGN (2 * MiB)
#endif

static size_t get_kernel_size(const struct fit_image_node *node)
{
	/*
	 * Since we don't have a way to determine the uncompressed size of the
	 * kernel, we have to keep as much memory as possible free for use by
	 * the kernel immediately after the end of the kernel image. The amount
	 * of space required will vary depending on selected features, and is
	 * effectively unbound.
	 */

	printk(BIOS_INFO,
	       "FIT: Leaving additional %u MiB of free space after kernel.\n",
	       MAX_KERNEL_SIZE >> 20);

	return node->size + MAX_KERNEL_SIZE;
}

/**
 * Place the region in free memory range.
 *
 * The caller has to set region->offset to the minimum allowed address.
 */
static bool fit_place_mem(const struct range_entry *r, void *arg)
{
	struct region *region = arg;
	resource_t start;

	if (range_entry_tag(r) != BM_MEM_RAM)
		return true;

	/* Section must be aligned at page boundary */
	start = ALIGN_UP(MAX(region->offset, range_entry_base(r)), SECTION_ALIGN);

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

	if (!config->fdt || !fdt) {
		printk(BIOS_CRIT, "Providing a valid FDT is mandatory to "
		       "boot a RISC-V kernel!\n");
		return false;
		/* TODO: Fall back to the ROM FDT? */
	}

	/* Update kernel size from image header, if possible */
	kernel->size = get_kernel_size(config->kernel);
	printk(BIOS_DEBUG, "FIT: Using kernel size of 0x%zx bytes\n",
	       kernel->size);

	/*
	 * The code assumes that bootmem_walk provides a sorted list of memory
	 * regions, starting from the lowest address.
	 * The order of the calls here doesn't matter, as the placement is
	 * enforced in the called functions.
	 * For details check code on top.
	 */
	kernel->offset = 0;
	if (!bootmem_walk(fit_place_mem, kernel))
		return false;

	/* Mark as reserved for future allocations. */
	bootmem_add_range(kernel->offset, kernel->size, BM_MEM_PAYLOAD);

	/* Place FDT and INITRD after kernel. */

	/* Place INITRD */
	if (config->ramdisk) {
		initrd->offset = kernel->offset + kernel->size;

		if (!bootmem_walk(fit_place_mem, initrd))
			return false;
		/* Mark as reserved for future allocations. */
		bootmem_add_range(initrd->offset, initrd->size, BM_MEM_PAYLOAD);
	}

	/* Place FDT */
	fdt->offset = kernel->offset + kernel->size;

	if (!bootmem_walk(fit_place_mem, fdt))
		return false;
	/* Mark as reserved for future allocations. */
	bootmem_add_range(fdt->offset, fdt->size, BM_MEM_PAYLOAD);

	/* Kernel expects FDT as argument */
	arg = (void *)fdt->offset;

	prog_set_entry(payload, (void *)kernel->offset, arg);

	bootmem_dump_ranges();

	return true;
}
