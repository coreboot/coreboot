/* SPDX-License-Identifier: GPL-2.0-only */

#include <endian.h>
#include <fmap_config.h>
#include <commonlib/bsd/cbfs_serialized.h>

struct cbfs_header header = {
	.magic = cpu_to_be32(CBFS_HEADER_MAGIC),
	.version = cpu_to_be32(CBFS_HEADER_VERSION),
	/*
	 * The offset and romsize fields within the master header are absolute
	 * values within the boot media. As such, romsize needs to reflect
	 * the end 'offset' for a CBFS. To achieve that the current buffer
	 * representing the CBFS region's size is added to the offset of
	 * the region within a larger image.
	 */
	.romsize = cpu_to_be32(FMAP_SECTION_COREBOOT_START + FMAP_SECTION_COREBOOT_SIZE
			       - FMAP_SECTION_FLASH_START),
	/*
	 * The 4 bytes are left out for two reasons:
	 * 1. the cbfs master header pointer resides there
	 * 2. some cbfs implementations assume that an image that resides
	 *    below 4GB has a bootblock and get confused when the end of the
	 *    image is at 4GB == 0.
	 */
	.bootblocksize = cpu_to_be32(4),
	.align = cpu_to_be32(CBFS_ALIGNMENT),
	.offset = cpu_to_be32(FMAP_SECTION_COREBOOT_START - FMAP_SECTION_FLASH_START),
	.architecture = cpu_to_be32(CBFS_ARCHITECTURE_UNKNOWN),
};
