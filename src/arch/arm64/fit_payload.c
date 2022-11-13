/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <commonlib/bsd/compression.h>
#include <console/console.h>
#include <bootmem.h>
#include <program_loading.h>
#include <string.h>
#include <lib.h>
#include <fit.h>
#include <endian.h>

#define MAX_KERNEL_SIZE (64*MiB)

struct arm64_kernel_header {
	u32 code0;
	u32 code1;
	u64 text_offset;
	u64 image_size;
	u64 flags;
	u64 res2;
	u64 res3;
	u64 res4;
	u32 magic;
#define KERNEL_HEADER_MAGIC  0x644d5241
	u32 res5;
};

static struct {
	union {
		struct arm64_kernel_header header;
		u8 raw[sizeof(struct arm64_kernel_header) + 0x100];
	};
#define SCRATCH_CANARY_VALUE 0xdeadbeef
	u32 canary;
} scratch;

/* Returns true if decompressing was successful and it looks like a kernel. */
static bool decompress_kernel_header(const struct fit_image_node *node)
{
	/* Partially decompress to get text_offset. Can't check for errors. */
	scratch.canary = SCRATCH_CANARY_VALUE;
	switch (node->compression) {
	case CBFS_COMPRESS_NONE:
		memcpy(scratch.raw, node->data, sizeof(scratch.raw));
		break;
	case CBFS_COMPRESS_LZMA:
		ulzman(node->data, node->size,
		       scratch.raw, sizeof(scratch.raw));
		break;
	case CBFS_COMPRESS_LZ4:
		ulz4fn(node->data, node->size,
		       scratch.raw, sizeof(scratch.raw));
		break;
	default:
		printk(BIOS_ERR, "Unsupported compression algorithm!\n");
		return false;
	}

	/* Should never happen, but if it does we'll want to know. */
	if (scratch.canary != SCRATCH_CANARY_VALUE)
		die("ERROR: Partial decompression ran over scratchbuf!\n");

	if (scratch.header.magic != KERNEL_HEADER_MAGIC) {
		printk(BIOS_ERR, "Invalid kernel magic: %#.8x\n != %#.8x\n",
		       scratch.header.magic, KERNEL_HEADER_MAGIC);
		return false;
	}

	/**
	 * Prior to v3.17, the endianness of text_offset was not specified.  In
	 * these cases image_size is zero and text_offset is 0x80000 in the
	 * endianness of the kernel.  Where image_size is non-zero image_size is
	 * little-endian and must be respected.  Where image_size is zero,
	 * text_offset can be assumed to be 0x80000.
	 */
	if (!scratch.header.image_size)
		scratch.header.text_offset = cpu_to_le64(0x80000);

	return true;
}

static size_t get_kernel_size(const struct fit_image_node *node)
{
	if (scratch.header.image_size)
		return le64_to_cpu(scratch.header.image_size);

	/**
	 * When image_size is zero, a bootloader should attempt to keep as much
	 * memory as possible free for use by the kernel immediately after the
	 * end of the kernel image. The amount of space required will vary
	 * depending on selected features, and is effectively unbound.
	 */

	printk(BIOS_WARNING, "FIT: image_size not set in kernel header.\n"
	       "Leaving additional %u MiB of free space after kernel.\n",
	       MAX_KERNEL_SIZE >> 20);

	return node->size + MAX_KERNEL_SIZE;
}

static bool fit_place_kernel(const struct range_entry *r, void *arg)
{
	struct region *region = arg;
	resource_t start;

	if (range_entry_tag(r) != BM_MEM_RAM)
		return true;

	/**
	 * The Image must be placed text_offset bytes from a 2MB aligned base
	 * address anywhere in usable system RAM and called there. The region
	 * between the 2 MB aligned base address and the start of the image has
	 * no special significance to the kernel, and may be used for other
	 * purposes.
	 *
	 * If the reserved memory (BL31 for example) is smaller than text_offset
	 * we can use the 2 MiB base address, otherwise use the next 2 MiB page.
	 * It's not mandatory, but wastes less memory below the kernel.
	 */
	start = ALIGN_DOWN(range_entry_base(r), 2 * MiB) +
		le64_to_cpu(scratch.header.text_offset);

	if (start < range_entry_base(r))
		start += 2 * MiB;
	/**
	 * At least image_size bytes from the start of the image must be free
	 * for use by the kernel.
	 */
	if (start + region->size < range_entry_end(r)) {
		region->offset = (size_t)start;
		return false;
	}

	return true;
}

/**
 * Place the region in free memory range.
 *
 * The caller has to set region->offset to the minimum allowed address.
 * The region->offset is usually 0 on kernel >v4.6 and kernel_base + kernel_size
 * on kernel <v4.6.
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
	bool place_anywhere;
	void *arg = NULL;

	if (!decompress_kernel_header(config->kernel)) {
		printk(BIOS_CRIT, "Payload doesn't look like an ARM64"
		       " kernel Image.\n");
		return false;
	}

	/* Update kernel size from image header, if possible */
	kernel->size = get_kernel_size(config->kernel);
	printk(BIOS_DEBUG, "FIT: Using kernel size of 0x%zx bytes\n",
	       kernel->size);

	/**
	 * The code assumes that bootmem_walk provides a sorted list of memory
	 * regions, starting from the lowest address.
	 * The order of the calls here doesn't matter, as the placement is
	 * enforced in the called functions.
	 * For details check code on top.
	 */

	if (!bootmem_walk(fit_place_kernel, kernel))
		return false;

	/* Mark as reserved for future allocations. */
	bootmem_add_range(kernel->offset, kernel->size, BM_MEM_PAYLOAD);

	/**
	 * NOTE: versions prior to v4.6 cannot make use of memory below the
	 * physical offset of the Image so it is recommended that the Image be
	 * placed as close as possible to the start of system RAM.
	 *
	 * For kernel <v4.6 the INITRD and FDT can't be placed below the kernel.
	 * In that case set region offset to an address on top of kernel.
	 */
	place_anywhere = !!(le64_to_cpu(scratch.header.flags) & (1 << 3));
	printk(BIOS_DEBUG, "FIT: Placing FDT and INITRD %s\n",
	       place_anywhere ? "anywhere" : "on top of kernel");

	/* Place INITRD */
	if (config->ramdisk) {
		if (place_anywhere)
			initrd->offset = 0;
		else
			initrd->offset = kernel->offset + kernel->size;

		if (!bootmem_walk(fit_place_mem, initrd))
			return false;
		/* Mark as reserved for future allocations. */
		bootmem_add_range(initrd->offset, initrd->size, BM_MEM_PAYLOAD);
	}

	/* Place FDT */
	if (place_anywhere)
		fdt->offset = 0;
	else
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
