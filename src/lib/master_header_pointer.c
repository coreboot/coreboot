/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/bsd/cbfs_serialized.h>
#include <endian.h>
#include <fmap_config.h>
#include <stdint.h>

#if ENV_X86
__attribute__((used, __section__(".header_pointer")))
#endif

#if FMAP_SECTION_COREBOOT_START < (0xffffffff - CONFIG_ROM_SIZE + 1)
#define COREBOOT_CBFS_START (0xffffffff - CONFIG_ROM_SIZE + 1 + FMAP_SECTION_COREBOOT_START)
#else
#define COREBOOT_CBFS_START FMAP_SECTION_COREBOOT_START
#endif

uint32_t header_pointer =
	cpu_to_le32(COREBOOT_CBFS_START + ALIGN_UP(sizeof(struct cbfs_file)
						   + sizeof("cbfs_master_header"),
						   CBFS_ATTRIBUTE_ALIGN));
