/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef COMMONLIB_TPM_LOG_SERIALIZED_H
#define COMMONLIB_TPM_LOG_SERIALIZED_H

#include <commonlib/bsd/helpers.h>
#include <stdint.h>

#define TPM_CB_LOG_DIGEST_MAX_LENGTH 64
#define TPM_CB_LOG_PCR_HASH_NAME 50
#define TPM_CB_LOG_PCR_HASH_LEN 10

struct tpm_cb_log_entry {
	uint32_t pcr;
	char digest_type[TPM_CB_LOG_PCR_HASH_LEN];
	uint8_t digest[TPM_CB_LOG_DIGEST_MAX_LENGTH];
	uint32_t digest_length;
	char name[TPM_CB_LOG_PCR_HASH_NAME];
} __packed;

struct tpm_cb_log_table {
	uint16_t max_entries;
	uint16_t num_entries;
	struct tpm_cb_log_entry entries[0]; /* Variable number of entries */
} __packed;

#endif
