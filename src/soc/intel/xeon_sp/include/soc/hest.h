/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _HEST_H_
#define _HEST_H_
#include <acpi/acpi.h>

#define MCE_ERR_POLL_MS_INTERVAL	1000
#define HEST_PCIE_RP_AER_DESC_TYPE	6
#define HEST_GHES_DESC_TYPE		9
#define GHES_MAX_RAW_DATA_LENGTH	(((CONFIG_ERROR_LOG_BUFFER_SIZE) >> 1) - 8)
#define GHEST_ERROR_STATUS_BLOCK_LENGTH	((CONFIG_ERROR_LOG_BUFFER_SIZE) >> 1)
#define GHEST_ASSIST			(1 << 2)
#define FIRMWARE_FIRST			(1 << 0)
#define MEM_VALID_BITS			0x66ff
#define PCIE_VALID_BITS			0xef
#define QWORD_ACCESS			4
#define NOTIFY_TYPE_SCI			3

/* Generic Error Source Descriptor */
typedef struct acpi_ghes_esd {
	u16 type;
	u16 source_id;
	u16 related_src_id;
	u8 flags;
	u8 enabled;
	u32 prealloc_erecords;
	u32 max_section_per_record;
} __packed acpi_ghes_esd_t;

typedef struct ghes_record {
	acpi_ghes_esd_t esd;
	u32 max_raw_data_length;
	acpi_addr64_t sts_addr;
	acpi_hest_hen_t notify;
	u32 err_sts_blk_len;
} __packed ghes_record_t;

unsigned long hest_create(unsigned long current, struct acpi_rsdp *rsdp);

#endif
