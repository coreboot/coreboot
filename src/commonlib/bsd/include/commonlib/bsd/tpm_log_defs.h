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

#define EV_PREBOOT_CERT			0x00000000
#define EV_POST_CODE			0x00000001
#define EV_UNUSED			0x00000002
#define EV_NO_ACTION			0x00000003
#define EV_SEPARATOR			0x00000004
#define EV_ACTION			0x00000005
#define EV_EVENT_TAG			0x00000006
#define EV_S_CRTM_CONTENTS		0x00000007
#define EV_S_CRTM_VERSION		0x00000008
#define EV_CPU_MICROCODE		0x00000009
#define EV_PLATFORM_CONFIG_FLAGS	0x0000000A
#define EV_TABLE_OF_DEVICES		0x0000000B
#define EV_COMPACT_HASH			0x0000000C
#define EV_IPL				0x0000000D
#define EV_IPL_PARTITION_DATA		0x0000000E
#define EV_NONHOST_CODE			0x0000000F
#define EV_NONHOST_CONFIG		0x00000010
#define EV_NONHOST_INFO			0x00000011
#define EV_OMIT_BOOT_DEVICE_EVENTS	0x00000012

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
