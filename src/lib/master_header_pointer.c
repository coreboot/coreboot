/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/bsd/cbfs_serialized.h>
#include <endian.h>
#include <fmap_config.h>
#include <stdint.h>

#if CONFIG(BOOTBLOCK_IN_CBFS)
__attribute__((used, __section__(".header_pointer")))
#endif

#define COREBOOT_CBFS_START (0xffffffff - CONFIG_ROM_SIZE + 1 + FMAP_SECTION_COREBOOT_START)

uint32_t header_pointer =
	cpu_to_le32(COREBOOT_CBFS_START + ALIGN_UP(sizeof(struct cbfs_file)
						   + sizeof("cbfs_master_header"),
						   CBFS_ATTRIBUTE_ALIGN));
