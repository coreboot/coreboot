/* Taken from depthcharge: src/base/device_tree.h */
/* SPDX-License-Identifier: GPL-2.0-or-later */

struct fdt_header {
	uint32_t magic;
	uint32_t totalsize;
	uint32_t structure_offset;
	uint32_t strings_offset;
	uint32_t reserve_map_offset;

	uint32_t version;
	uint32_t last_compatible_version;

	uint32_t boot_cpuid_phys;

	uint32_t strings_size;
	uint32_t structure_size;
};

#define FDT_HEADER_MAGIC	0xd00dfeed
