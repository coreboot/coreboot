/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_QUALCOMM_CDT_H_
#define _SOC_QUALCOMM_CDT_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#define CDT_REGION_NAME "RW_CDT"
#define CDT_MAGIC 0x00544443

struct cdt_header {
	uint32_t magic;
	uint16_t version;
	uint32_t reserved1;
	uint32_t reserved2;
} __packed;

struct cdb_meta {
	uint16_t offset;
	uint16_t size;
};

#define CDT_BLOCK_INDEX_PLATFORM_ID  0
#define CDT_BLOCK_INDEX_DDR          1

struct platform_id_cdt {
	uint8_t version;
	uint8_t platform;
	uint8_t hw_version_major;
	uint8_t hw_version_minor;
	uint8_t subtype;
	uint8_t num_kvps;
};

#define SOC_BOOT_CONFIG_FAST_BOOT_MASK	0x3E
#define SOC_BOOT_CONFIG_FAST_BOOT_SHIFT	1

#define BOOT_OPTION_QSPI_NVME		0x0
#define BOOT_OPTION_UFS_HS_G1		0x1
#define BOOT_OPTION_SPI_NVME		0x2
#define BOOT_OPTION_QSPI_NVME_ALT	0x3
#define BOOT_OPTION_DUAL_SPI_NVME	0x4
#define BOOT_OPTION_DUAL_SPI_UFS	0x5
#define BOOT_OPTION_QSPI_UFS		0x6
#define BOOT_OPTION_SPI_UFS		0x7

#define CALYPSO_STORAGE_TYPE_UNKNOWN	0
#define CALYPSO_STORAGE_TYPE_NVME	1
#define CALYPSO_STORAGE_TYPE_UFS	2

/*
 * CDT fw_config 64-bit layout:
 *   [15:0]  : Platform ID (16-bit)
 *   [31:16] : SoC ID (16-bit)
 *   [39:32] : Fast Boot Config (8-bit)
 *   [63:40] : Reserved
 */
#define CDT_FW_CFG_SOC_ID_SHIFT		16
#define CDT_FW_CFG_BOOT_CFG_SHIFT	32

#define EXTRACT_FIELD(val, mask, shift) \
	(((val) & (mask)) >> (shift))

#define CDT_COMBINE_SOC_PLATFORM_ID(soc, plat) \
	(((uint32_t)(soc) << CDT_FW_CFG_SOC_ID_SHIFT) | (uint16_t)(plat))

#define CDT_PACK_FW_CONFIG(boot_cfg, soc, plat) \
	(((uint64_t)(boot_cfg) << CDT_FW_CFG_BOOT_CFG_SHIFT) | \
	 (uint64_t)CDT_COMBINE_SOC_PLATFORM_ID(soc, plat))

ssize_t cdt_read(void *buffer, size_t buffer_size);
uint16_t cdt_get_platform_id(void);
uint32_t cdt_get_hw_version(void);
uint8_t platform_get_fast_boot(void);
bool platform_boot_media_is_nvme(void);

#endif /* _SOC_QUALCOMM_CDT_H_ */
