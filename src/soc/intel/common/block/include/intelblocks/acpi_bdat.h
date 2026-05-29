/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SOC_INTEL_COMMON_BLOCK_ACPI_BDAT_H
#define SOC_INTEL_COMMON_BLOCK_ACPI_BDAT_H

#include <acpi/acpi.h>
#include <fsp/api.h>
#include <fsp/util.h>

#define BDAT_PRIMARY_VERSION      4
#define BDAT_SECONDARY_VERSION    0

/*
 * The header information is from
 * BDAT interface spec 4.0.
 */
struct bdat_header_structure {
	/* "BDATHEAD" */
	uint8_t bios_data_sig[8];
	uint32_t bios_data_size;
	uint16_t crc16;
	uint16_t reserved;
	uint16_t primary_rev;
	uint16_t secondary_rev;
	uint32_t oem_offset;
	uint32_t reserved1;
	uint32_t reserved2;
} __packed;

struct bdat_schema_list_structure {
	uint16_t schema_list_length;
	uint16_t reserved1;
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint8_t reserved2;
} __packed;

struct bdat_structure {
	struct bdat_header_structure header;
	struct bdat_schema_list_structure schemas;
} __packed;

#define MAX_SCHEMA_LIST_LENGTH	10

typedef struct {
	const uint32_t *address;
	uint32_t size;
} schema_record;

/*
 * Not strictly part of the BDAT spec. This HOB is used to identify which HOBs
 * contain data that should be copied into the final BDAT structure as a schema.
 */
struct bdat_schema_list_hob {
	/*
	 * Number of HOBs that exist which contain data that should be
	 * copied to the BDAT structure.
	 */
	uint16_t schema_hob_count;
	uint16_t reserved;
	/*
	 * Array of GUIDs which identify HOBs that contain data
	 * that should be copied into the BDAT structure.  The contents
	 * of the HOB contains a schema. The GUID for the HOB is the
	 * same as the SchemaId GUID.
	 */
	efi_guid_t schema_hob_guids[MAX_SCHEMA_LIST_LENGTH];
} __packed;

#endif	/* SOC_INTEL_COMMON_BLOCK_ACPI_BDAT_H */
