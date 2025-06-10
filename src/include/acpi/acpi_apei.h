/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Contains ACPI Platform Error Interfaces (APEI) definitions and declarations.
 * These are mostly used by:
 * - BERT (Boot Error Record Table) ACPI Table.
 * - HEST (Hardware Error Source Table) ACPI Table.
 */

#ifndef _ACPI_APEI_H_
#define _ACPI_APEI_H_

#include <acpi/acpi.h>

/* Generic Hardware Error Source Descriptor */
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

uintptr_t acpi_soc_fill_hest(acpi_hest_t *hest, uintptr_t current, void *log_mem);

#endif
