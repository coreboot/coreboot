/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef COMMONLIB_BSD_TPM_LOG_DEFS_H
#define COMMONLIB_BSD_TPM_LOG_DEFS_H

#include <commonlib/helpers.h>
#include <stdint.h>

#define TCPA_SPEC_ID_EVENT_SIGNATURE    "Spec ID Event00"
#define TCG_EFI_SPEC_ID_EVENT_SIGNATURE "Spec ID Event03"

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

#endif
