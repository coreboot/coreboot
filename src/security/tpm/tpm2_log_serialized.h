/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef __TPM2_LOG_SERIALIZED_H__
#define __TPM2_LOG_SERIALIZED_H__

#include <commonlib/bsd/tpm_log_defs.h>

#define TPM_20_SPEC_ID_EVENT_SIGNATURE "Spec ID Event03"
#define TPM_20_LOG_DATA_MAX_LENGTH 50

#define TPM_20_LOG_VI_MAGIC 0x32544243 /* "CBT2" in LE */
#define TPM_20_LOG_VI_MAJOR 1
#define TPM_20_LOG_VI_MINOR 0

/*
 * TPM2.0 log entries can't be generally represented as C structures due to
 * varying number of digests and their sizes. However, it works as long as
 * we're only using single kind of digests.
 */
#if CONFIG(TPM_LOG_TPM2)
#  if CONFIG(TPM_HASH_SHA1)
#    define TPM_20_LOG_DIGEST_MAX_LENGTH SHA1_DIGEST_SIZE
#  endif
#  if CONFIG(TPM_HASH_SHA256)
#    define TPM_20_LOG_DIGEST_MAX_LENGTH SHA256_DIGEST_SIZE
#  endif
#  if CONFIG(TPM_HASH_SHA384)
#    define TPM_20_LOG_DIGEST_MAX_LENGTH SHA384_DIGEST_SIZE
#  endif
#  if CONFIG(TPM_HASH_SHA512)
#    define TPM_20_LOG_DIGEST_MAX_LENGTH SHA512_DIGEST_SIZE
#  endif

#  ifndef TPM_20_LOG_DIGEST_MAX_LENGTH
#    error "Misconfiguration: failed to determine TPM hashing algorithm"
#  endif
#else
#  define TPM_20_LOG_DIGEST_MAX_LENGTH 1 /* To avoid compilation error */
#endif

/* TCG_PCR_EVENT2 */
struct tpm_2_log_entry {
	uint32_t pcr;
	uint32_t event_type;
	uint32_t digest_count; /* Always 1 in current implementation */
	uint16_t digest_type;
	uint8_t digest[TPM_20_LOG_DIGEST_MAX_LENGTH];
	uint32_t data_length;
	uint8_t data[TPM_20_LOG_DATA_MAX_LENGTH];
} __packed;

struct tpm_2_vendor {
	uint8_t reserved;
	uint8_t version_major;
	uint8_t version_minor;
	uint32_t magic;
	uint16_t max_entries;
	uint16_t num_entries;
	uint32_t entry_size;
} __packed;

struct tpm_2_log_table {
	struct tcg_efi_spec_id_event header; /* TCG_PCR_EVENT actually */
	struct tpm_digest_sizes digest_sizes[1];
	uint8_t vendor_info_size;
	struct tpm_2_vendor vendor;
	struct tpm_2_log_entry entries[0]; /* Variable number of entries */
} __packed;

#endif
