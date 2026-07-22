/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_QUALCOMM_CDT_H_
#define _SOC_QUALCOMM_CDT_H_

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

ssize_t cdt_read(void *buffer, size_t buffer_size);
uint16_t cdt_get_platform_id(void);
uint32_t cdt_get_hw_version(void);

#endif /* _SOC_QUALCOMM_CDT_H_ */
