/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef COMMONLIB_BSD_TPM_LOG_DEFS_H
#define COMMONLIB_BSD_TPM_LOG_DEFS_H

#include <commonlib/helpers.h>
#include <stdint.h>

#define TCPA_SPEC_ID_EVENT_SIGNATURE    "Spec ID Event00"
#define TCG_EFI_SPEC_ID_EVENT_SIGNATURE "Spec ID Event03"

struct tcpa_log_entry {
	uint32_t pcr;
	uint32_t event_type;
	uint8_t digest[20];
	uint32_t event_data_size;
	uint8_t event[0];
} __packed;

struct tcpa_spec_entry {
	struct tcpa_log_entry entry;
	uint8_t signature[16];
	uint32_t platform_class;
	uint8_t spec_version_minor;
	uint8_t spec_version_major;
	uint8_t spec_errata;
	uint8_t reserved;
	uint8_t vendor_info_size;
	uint8_t vendor_info[0];
} __packed;

#define TPM2_ALG_ERROR   0x0000
#define TPM2_ALG_HMAC    0x0005
#define TPM2_ALG_NULL    0x0010
#define TPM2_ALG_SHA1    0x0004
#define TPM2_ALG_SHA256  0x000B
#define TPM2_ALG_SHA384  0x000C
#define TPM2_ALG_SHA512  0x000D
#define TPM2_ALG_SM3_256 0x0012

#define SHA1_DIGEST_SIZE    20
#define SHA256_DIGEST_SIZE  32
#define SHA384_DIGEST_SIZE  48
#define SHA512_DIGEST_SIZE  64
#define SM3_256_DIGEST_SIZE 32

enum ev_enum {
	EV_PREBOOT_CERT,
	EV_POST_CODE,
	EV_UNUSED,
	EV_NO_ACTION,
	EV_SEPARATOR,
	EV_ACTION,
	EV_EVENT_TAG,
	EV_S_CRTM_CONTENTS,
	EV_S_CRTM_VERSION,
	EV_CPU_MICROCODE,
	EV_PLATFORM_CONFIG_FLAGS,
	EV_TABLE_OF_DEVICES,
	EV_COMPACT_HASH,
	EV_IPL,
	EV_IPL_PARTITION_DATA,
	EV_NONHOST_CODE,
	EV_NONHOST_CONFIG,
	EV_NONHOST_INFO,
	EV_OMIT_BOOT_DEVICE_EVENTS
};

struct spec_id_event_data {
	char signature[16];
	uint32_t platform_class;
	uint8_t spec_version_minor;
	uint8_t spec_version_major;
	uint8_t spec_errata;
	uint8_t reserved;
	uint8_t vendor_info_size;
} __packed;

union tpm_hash_digest {
	uint8_t sha1[SHA1_DIGEST_SIZE];
	uint8_t sha256[SHA256_DIGEST_SIZE];
	uint8_t sm3_256[SM3_256_DIGEST_SIZE];
	uint8_t sha384[SHA384_DIGEST_SIZE];
	uint8_t sha512[SHA512_DIGEST_SIZE];
};

struct tpm_hash_algorithm {
	uint16_t hashAlg;
	union tpm_hash_digest digest;
} __packed;

struct tcg_pcr_event2_header {
	uint32_t pcr_index;
	uint32_t event_type;
	uint32_t digest_count;
	uint8_t digests[0];
	/* uint32_t event_size; */
	/* uint8_t event[0]; */
} __packed;

struct tpm_digest_sizes {
	uint16_t alg_id;
	uint16_t digest_size;
} __packed;

struct tcg_efi_spec_id_event {
	uint32_t pcr_index;
	uint32_t event_type;
	uint8_t digest[20];
	uint32_t event_size;
	uint8_t signature[16];
	uint32_t platform_class;
	uint8_t spec_version_minor;
	uint8_t spec_version_major;
	uint8_t spec_errata;
	uint8_t uintn_size;
	uint32_t num_of_algorithms;
	struct tpm_digest_sizes digest_sizes[0]; /* variable number of members */
	/* uint8_t vendor_info_size; */
	/* uint8_t vendor_info[vendor_info_size]; */
} __packed;

static const char *tpm_event_types[] __maybe_unused = {
	[EV_PREBOOT_CERT]		= "Reserved",
	[EV_POST_CODE]			= "POST code",
	[EV_UNUSED]			= "Unused",
	[EV_NO_ACTION]			= "No action",
	[EV_SEPARATOR]			= "Separator",
	[EV_ACTION]			= "Action",
	[EV_EVENT_TAG]			= "Event tag",
	[EV_S_CRTM_CONTENTS]		= "S-CRTM contents",
	[EV_S_CRTM_VERSION]		= "S-CRTM version",
	[EV_CPU_MICROCODE]		= "CPU microcode",
	[EV_PLATFORM_CONFIG_FLAGS]	= "Platform configuration flags",
	[EV_TABLE_OF_DEVICES]		= "Table of devices",
	[EV_COMPACT_HASH]		= "Compact hash",
	[EV_IPL]			= "IPL",
	[EV_IPL_PARTITION_DATA]		= "IPL partition data",
	[EV_NONHOST_CODE]		= "Non-host code",
	[EV_NONHOST_CONFIG]		= "Non-host configuration",
	[EV_NONHOST_INFO]		= "Non-host information",
	[EV_OMIT_BOOT_DEVICE_EVENTS]	= "Omit boot device events",
};

#endif
