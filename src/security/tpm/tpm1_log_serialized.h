/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef __TPM1_LOG_SERIALIZED_H__
#define __TPM1_LOG_SERIALIZED_H__

#include <commonlib/bsd/helpers.h>
#include <commonlib/bsd/tpm_log_defs.h>
#include <stdint.h>

#define TPM_1_LOG_DIGEST_MAX_LENGTH 20
#define TPM_1_LOG_DATA_MAX_LENGTH 50

#define TPM_1_LOG_VI_MAGIC 0x31544243 /* "CBT1" in LE */
#define TPM_1_LOG_VI_MAJOR 1
#define TPM_1_LOG_VI_MINOR 0

struct tpm_1_log_entry {
	uint32_t pcr;
	uint32_t event_type;
	uint8_t digest[TPM_1_LOG_DIGEST_MAX_LENGTH];
	uint32_t data_length;
	uint8_t data[TPM_1_LOG_DATA_MAX_LENGTH];
} __packed;

struct tpm_1_vendor {
	uint8_t reserved;
	uint8_t version_major;
	uint8_t version_minor;
	uint32_t magic;
	uint16_t max_entries;
	uint16_t num_entries;
	uint32_t entry_size;
} __packed;

struct tpm_1_log_table {
	/* The first entry of the log is inlined and describes the log itself */
	uint32_t pcr;
	uint32_t event_type;
	uint8_t digest[TPM_1_LOG_DIGEST_MAX_LENGTH];
	uint32_t spec_id_size;
	struct spec_id_event_data spec_id;
	struct tpm_1_vendor vendor;

	struct tpm_1_log_entry entries[]; /* Variable number of entries */
} __packed;

#endif
