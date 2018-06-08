/*
 * Copyright 2016 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef TCG2_TSS_STRUCTURES_H_
#define TCG2_TSS_STRUCTURES_H_

/*
 * This file includes a subset of definitions of TPM protocol version 2.x
 * constants and structures needed for functions used in coreboot.
 */
#include <stdint.h>
#include <compiler.h>
#include <types.h>
#include "../common/tss_common.h"

/* This should be plenty for what firmware needs. */
#define TPM_BUFFER_SIZE 256

/* Some TPM2 return codes used in this library. */
#define TPM2_RC_SUCCESS    0
#define TPM2_RC_NV_DEFINED 0x14c

/* Basic TPM2 types. */
typedef uint16_t TPM_SU;
typedef uint16_t TPM_ALG_ID;
typedef uint32_t TPM_HANDLE;
typedef uint32_t TPM_RC;
typedef uint8_t TPMI_YES_NO;
typedef TPM_ALG_ID TPMI_ALG_HASH;
typedef TPM_HANDLE TPMI_DH_PCR;
typedef TPM_HANDLE TPMI_RH_NV_INDEX;
typedef TPM_HANDLE TPMI_RH_ENABLES;
typedef TPM_HANDLE TPMI_SH_AUTH_SESSION;
typedef TPM_HANDLE TPM_RH;

/* Some hardcoded algorithm values. */
#define TPM_ALG_HMAC   ((TPM_ALG_ID)0x0005)
#define TPM_ALG_NULL   ((TPM_ALG_ID)0x0010)
#define TPM_ALG_SHA1   ((TPM_ALG_ID)0x0004)
#define TPM_ALG_SHA256 ((TPM_ALG_ID)0x000b)

#define SHA256_DIGEST_SIZE 32

/* Some hardcoded hierarchies. */
#define TPM_RH_NULL         0x40000007
#define TPM_RS_PW           0x40000009
#define TPM_RH_PLATFORM     0x4000000C

typedef uint32_t TPM_CC;

typedef struct {
	uint16_t      size;
	uint8_t       *buffer;
} TPM2B;

/* Relevant TPM Command's structures. */
/* Common command/response header. */
struct tpm_header {
	uint16_t tpm_tag;
	uint32_t tpm_size;
	TPM_CC tpm_code;
} __packed;

/* TPM command codes. */
#define TPM2_Hierarchy_Control ((TPM_CC)0x00000121)
#define TPM2_Clear             ((TPM_CC)0x00000126)
#define TPM2_NV_DefineSpace    ((TPM_CC)0x0000012A)
#define TPM2_NV_Write          ((TPM_CC)0x00000137)
#define TPM2_NV_WriteLock      ((TPM_CC)0x00000138)
#define TPM2_SelfTest          ((TPM_CC)0x00000143)
#define TPM2_Startup           ((TPM_CC)0x00000144)
#define TPM2_NV_Read           ((TPM_CC)0x0000014E)
#define TPM2_GetCapability     ((TPM_CC)0x0000017A)
#define TPM2_PCR_Extend        ((TPM_CC)0x00000182)
/* TPM2 specifies vendor commands need to have this bit set. Vendor command
   space is defined by the lower 16 bits. */
#define TPM_CC_VENDOR_BIT_MASK 0x20000000

/* Startup values. */
#define TPM_SU_CLEAR 0
#define TPM_SU_STATE 1

#define TPM_HT_PCR             0x00
#define TPM_HT_NV_INDEX        0x01
#define TPM_HT_HMAC_SESSION    0x02
#define TPM_HT_POLICY_SESSION  0x03

#define HR_SHIFT               24
#define HR_PCR                (TPM_HT_PCR <<  HR_SHIFT)
#define HR_HMAC_SESSION       (TPM_HT_HMAC_SESSION <<  HR_SHIFT)
#define HR_POLICY_SESSION     (TPM_HT_POLICY_SESSION <<  HR_SHIFT)
#define HR_TRANSIENT          (TPM_HT_TRANSIENT <<  HR_SHIFT)
#define HR_PERSISTENT         (TPM_HT_PERSISTENT <<  HR_SHIFT)
#define HR_NV_INDEX           (TPM_HT_NV_INDEX <<  HR_SHIFT)
#define HR_PERMANENT          (TPM_HT_PERMANENT <<  HR_SHIFT)
#define PCR_FIRST             (HR_PCR + 0)
#define PCR_LAST              (PCR_FIRST + IMPLEMENTATION_PCR-1)
#define HMAC_SESSION_FIRST    (HR_HMAC_SESSION + 0)
#define HMAC_SESSION_LAST     (HMAC_SESSION_FIRST+MAX_ACTIVE_SESSIONS-1)
#define LOADED_SESSION_FIRST  HMAC_SESSION_FIRST
#define LOADED_SESSION_LAST   HMAC_SESSION_LAST
#define POLICY_SESSION_FIRST  (HR_POLICY_SESSION + 0)
#define POLICY_SESSION_LAST   (POLICY_SESSION_FIRST + MAX_ACTIVE_SESSIONS-1)
#define TRANSIENT_FIRST       (HR_TRANSIENT + 0)
#define ACTIVE_SESSION_FIRST  POLICY_SESSION_FIRST
#define ACTIVE_SESSION_LAST   POLICY_SESSION_LAST
#define TRANSIENT_LAST        (TRANSIENT_FIRST+MAX_LOADED_OBJECTS-1)
#define PERSISTENT_FIRST      (HR_PERSISTENT + 0)
#define PERSISTENT_LAST       (PERSISTENT_FIRST + 0x00FFFFFF)
#define PLATFORM_PERSISTENT   (PERSISTENT_FIRST + 0x00800000)
#define NV_INDEX_FIRST        (HR_NV_INDEX + 0)
#define NV_INDEX_LAST         (NV_INDEX_FIRST + 0x00FFFFFF)
#define PERMANENT_FIRST       TPM_RH_FIRST
#define PERMANENT_LAST        TPM_RH_LAST

/* Tpm2 command tags. */
#define TPM_ST_NO_SESSIONS 0x8001
#define TPM_ST_SESSIONS    0x8002

/* Values copied from tpm2/tpm_types.h */
#define RC_VER1                                         0x100
#define TPM_RC_INITIALIZE         ((TPM_RC)(RC_VER1 + 0x000))
#define TPM_RC_NV_UNINITIALIZED	  ((TPM_RC)(RC_VER1 + 0x04A))

/*
 * Cr50 returns this code when an attempt is made to read an NV location which
 * has not yet been defined. This is an aggregation of various return code
 * extensions which may or may not match if a different TPM2 device is
 * used.
 */
#define TPM_RC_CR50_NV_UNDEFINED  0x28b

/* TPM command structures. */

struct tpm2_startup {
	TPM_SU  startup_type;
};

/* Various TPM capability types to use when querying the device. */
typedef uint32_t TPM_CAP;
#define TPM_CAP_TPM_PROPERTIES   ((TPM_CAP)0x00000006)

typedef TPM_HANDLE TPMI_RH_NV_AUTH;
typedef TPM_HANDLE TPMI_RH_NV_INDEX;

/* TPM Property capability constants. */
typedef uint32_t TPM_PT;
#define PT_GROUP                                   0x00000100
#define PT_VAR                                 (PT_GROUP * 2)
#define TPM_PT_PERMANENT               ((TPM_PT)(PT_VAR + 0))

/* Structures of payloads of various TPM2 commands. */
struct tpm2_get_capability {
	TPM_CAP capability;
	uint32_t property;
	uint32_t propertyCount;
};

/* get_capability response when PT_PERMANENT is requested. */
typedef struct {
	uint32_t ownerAuthSet       : 1;
	uint32_t endorsementAuthSet : 1;
	uint32_t lockoutAuthSet     : 1;
	uint32_t reserved3_7        : 5;
	uint32_t disableClear       : 1;
	uint32_t inLockout          : 1;
	uint32_t tpmGeneratedEPS    : 1;
	uint32_t reserved11_31      : 21;
} TPMA_PERMANENT;

typedef struct {
	uint32_t TPMA_NV_PPWRITE        : 1;
	uint32_t TPMA_NV_OWNERWRITE     : 1;
	uint32_t TPMA_NV_AUTHWRITE      : 1;
	uint32_t TPMA_NV_POLICYWRITE    : 1;
	uint32_t TPMA_NV_COUNTER        : 1;
	uint32_t TPMA_NV_BITS           : 1;
	uint32_t TPMA_NV_EXTEND         : 1;
	uint32_t reserved7_9            : 3;
	uint32_t TPMA_NV_POLICY_DELETE  : 1;
	uint32_t TPMA_NV_WRITELOCKED    : 1;
	uint32_t TPMA_NV_WRITEALL       : 1;
	uint32_t TPMA_NV_WRITEDEFINE    : 1;
	uint32_t TPMA_NV_WRITE_STCLEAR  : 1;
	uint32_t TPMA_NV_GLOBALLOCK     : 1;
	uint32_t TPMA_NV_PPREAD         : 1;
	uint32_t TPMA_NV_OWNERREAD      : 1;
	uint32_t TPMA_NV_AUTHREAD       : 1;
	uint32_t TPMA_NV_POLICYREAD     : 1;
	uint32_t reserved20_24          : 5;
	uint32_t TPMA_NV_NO_DA          : 1;
	uint32_t TPMA_NV_ORDERLY        : 1;
	uint32_t TPMA_NV_CLEAR_STCLEAR  : 1;
	uint32_t TPMA_NV_READLOCKED     : 1;
	uint32_t TPMA_NV_WRITTEN        : 1;
	uint32_t TPMA_NV_PLATFORMCREATE : 1;
	uint32_t TPMA_NV_READ_STCLEAR   : 1;
} TPMA_NV;

typedef union {
	struct {
		uint16_t  size;
		const uint8_t   *buffer;
	} t;
	TPM2B b;
} TPM2B_DIGEST;

typedef TPM2B_DIGEST TPM2B_AUTH;
typedef TPM2B_DIGEST TPM2B_NONCE;

typedef struct {
	TPM_PT  property;
	uint32_t  value;
} TPMS_TAGGED_PROPERTY;

#define MAX_CAP_DATA (TPM_BUFFER_SIZE - sizeof(struct tpm_header) - \
		      sizeof(TPMI_YES_NO) - sizeof(TPM_CAP) - sizeof(uint32_t))
#define MAX_TPM_PROPERTIES  (MAX_CAP_DATA/sizeof(TPMS_TAGGED_PROPERTY))

/* Somewhat arbitrary, leave enough room for command wrappers. */
#define MAX_NV_BUFFER_SIZE (TPM_BUFFER_SIZE - sizeof(struct tpm_header) - 50)

typedef struct {
	uint32_t              count;
	TPMS_TAGGED_PROPERTY  tpmProperty[MAX_TPM_PROPERTIES];
} TPML_TAGGED_TPM_PROPERTY;

typedef union {
	TPML_TAGGED_TPM_PROPERTY  tpmProperties;
} TPMU_CAPABILITIES;

typedef struct {
	TPM_CAP            capability;
	TPMU_CAPABILITIES  data;
} TPMS_CAPABILITY_DATA;

struct get_cap_response {
	TPMI_YES_NO more_data;
	TPMS_CAPABILITY_DATA cd;
};

typedef struct {
	TPMI_RH_NV_INDEX  nvIndex;
	TPMI_ALG_HASH     nameAlg;
	TPMA_NV           attributes;
	TPM2B_DIGEST      authPolicy;
	uint16_t          dataSize;
} TPMS_NV_PUBLIC;

typedef union {
	struct {
		uint16_t        size;
		TPMS_NV_PUBLIC  nvPublic;
	} t;
	TPM2B b;
} TPM2B_NV_PUBLIC;

typedef union {
	struct {
		uint16_t  size;
		const uint8_t   *buffer;
	} t;
	TPM2B b;
} TPM2B_MAX_NV_BUFFER;

/*
 * This is a union, but as of now we support just one digest - sha256, so
 * there is just one element.
 */
typedef union {
	uint8_t  sha256[SHA256_DIGEST_SIZE];
} TPMU_HA;

typedef struct {
	TPMI_ALG_HASH  hashAlg;
	TPMU_HA        digest;
} TPMT_HA;

typedef struct {
	uint32_t   count;
	TPMT_HA  digests[1];  /* Limit max number of hashes to 1. */
} TPML_DIGEST_VALUES;

struct nv_read_response {
	uint32_t params_size;
	TPM2B_MAX_NV_BUFFER buffer;
};

struct vendor_command_response {
	uint16_t vc_subcommand;
	union {
		uint8_t num_restored_headers;
	};
};

struct tpm2_session_attrs {
	uint8_t continueSession : 1;
	uint8_t auditExclusive  : 1;
	uint8_t auditReset      : 1;
	uint8_t reserved3_4     : 2;
	uint8_t decrypt         : 1;
	uint8_t encrypt         : 1;
	uint8_t audit           : 1;
};

/*
 * TPM session header for commands requiring session information. Also
 * included in the responses to those commands.
 */
struct tpm2_session_header {
	uint32_t session_handle;
	uint16_t nonce_size;
	uint8_t *nonce;
	union {
		struct tpm2_session_attrs session_attr_bits;
		uint8_t session_attrs;
	}  __packed;
	uint16_t auth_size;
	uint8_t *auth;
};

struct tpm2_response {
	struct tpm_header hdr;
	union {
		struct get_cap_response gc;
		struct nv_read_response nvr;
		struct tpm2_session_header def_space;
		struct vendor_command_response vcr;
	};
};

struct tpm2_nv_define_space_cmd {
	TPM2B_AUTH auth;
	TPMS_NV_PUBLIC publicInfo;
};

struct tpm2_nv_write_cmd {
	TPMI_RH_NV_INDEX nvIndex;
	TPM2B_MAX_NV_BUFFER data;
	uint16_t offset;
};

struct tpm2_self_test {
	TPMI_YES_NO yes_no;
};

struct tpm2_nv_read_cmd {
	TPMI_RH_NV_INDEX nvIndex;
	uint16_t size;
	uint16_t offset;
};

struct tpm2_nv_write_lock_cmd {
	TPMI_RH_NV_INDEX nvIndex;
};

struct tpm2_pcr_extend_cmd {
	TPMI_DH_PCR pcrHandle;
	TPML_DIGEST_VALUES digests;
};

struct tpm2_hierarchy_control_cmd {
	TPMI_RH_ENABLES enable;
	TPMI_YES_NO state;
};

#endif // TCG2_TSS_STRUCTURES_H_
