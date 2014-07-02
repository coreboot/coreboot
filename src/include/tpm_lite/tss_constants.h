/* Copyright (c) 2013 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * Some TPM constants and type definitions for standalone compilation for use
 * in the firmware
 */
#ifndef VBOOT_REFERENCE_TSS_CONSTANTS_H_
#define VBOOT_REFERENCE_TSS_CONSTANTS_H_
#include <stdint.h>

#define TPM_MAX_COMMAND_SIZE 4096
#define TPM_LARGE_ENOUGH_COMMAND_SIZE 256  /* saves space in the firmware */
#define TPM_PUBEK_SIZE 256
#define TPM_PCR_DIGEST 20

#define TPM_E_NON_FATAL 0x800

#define TPM_SUCCESS ((uint32_t)0x00000000)

#define TPM_E_AREA_LOCKED           ((uint32_t)0x0000003c)
#define TPM_E_BADINDEX              ((uint32_t)0x00000002)
#define TPM_E_BAD_PRESENCE          ((uint32_t)0x0000002d)
#define TPM_E_IOERROR               ((uint32_t)0x0000001f)
#define TPM_E_INVALID_POSTINIT      ((uint32_t)0x00000026)
#define TPM_E_MAXNVWRITES           ((uint32_t)0x00000048)
#define TPM_E_OWNER_SET             ((uint32_t)0x00000014)

#define TPM_E_NEEDS_SELFTEST ((uint32_t)(TPM_E_NON_FATAL + 1))
#define TPM_E_DOING_SELFTEST ((uint32_t)(TPM_E_NON_FATAL + 2))

#define TPM_E_ALREADY_INITIALIZED    ((uint32_t)0x00005000)  /* vboot local */
#define TPM_E_INTERNAL_INCONSISTENCY ((uint32_t)0x00005001)  /* vboot local */
#define TPM_E_MUST_REBOOT            ((uint32_t)0x00005002)  /* vboot local */
#define TPM_E_CORRUPTED_STATE        ((uint32_t)0x00005003)  /* vboot local */
#define TPM_E_COMMUNICATION_ERROR    ((uint32_t)0x00005004)  /* vboot local */
#define TPM_E_RESPONSE_TOO_LARGE     ((uint32_t)0x00005005)  /* vboot local */
#define TPM_E_NO_DEVICE              ((uint32_t)0x00005006)  /* vboot local */
#define TPM_E_INPUT_TOO_SMALL        ((uint32_t)0x00005007)  /* vboot local */
#define TPM_E_WRITE_FAILURE          ((uint32_t)0x00005008)  /* vboot local */
#define TPM_E_READ_EMPTY             ((uint32_t)0x00005009)  /* vboot local */
#define TPM_E_READ_FAILURE           ((uint32_t)0x0000500a)  /* vboot local */

#define TPM_NV_INDEX0 ((uint32_t)0x00000000)
#define TPM_NV_INDEX_LOCK ((uint32_t)0xffffffff)
#define TPM_NV_PER_GLOBALLOCK (((uint32_t)1)<<15)
#define TPM_NV_PER_PPWRITE (((uint32_t)1)<<0)
#define TPM_NV_PER_READ_STCLEAR (((uint32_t)1)<<31)
#define TPM_NV_PER_WRITE_STCLEAR (((uint32_t)1)<<14)

#define TPM_TAG_RQU_COMMAND       ((uint16_t) 0xc1)
#define TPM_TAG_RQU_AUTH1_COMMAND ((uint16_t) 0xc2)
#define TPM_TAG_RQU_AUTH2_COMMAND ((uint16_t) 0xc3)

#define TPM_TAG_RSP_COMMAND       ((uint16_t) 0xc4)
#define TPM_TAG_RSP_AUTH1_COMMAND ((uint16_t) 0xc5)
#define TPM_TAG_RSP_AUTH2_COMMAND ((uint16_t) 0xc6)

typedef uint8_t TSS_BOOL;
typedef uint16_t TPM_STRUCTURE_TAG;

typedef struct tdTPM_PERMANENT_FLAGS
{
	TPM_STRUCTURE_TAG tag;
	TSS_BOOL disable;
	TSS_BOOL ownership;
	TSS_BOOL deactivated;
	TSS_BOOL readPubek;
	TSS_BOOL disableOwnerClear;
	TSS_BOOL allowMaintenance;
	TSS_BOOL physicalPresenceLifetimeLock;
	TSS_BOOL physicalPresenceHWEnable;
	TSS_BOOL physicalPresenceCMDEnable;
	TSS_BOOL CEKPUsed;
	TSS_BOOL TPMpost;
	TSS_BOOL TPMpostLock;
	TSS_BOOL FIPS;
	TSS_BOOL Operator;
	TSS_BOOL enableRevokeEK;
	TSS_BOOL nvLocked;
	TSS_BOOL readSRKPub;
	TSS_BOOL tpmEstablished;
	TSS_BOOL maintenanceDone;
	TSS_BOOL disableFullDALogicInfo;
} TPM_PERMANENT_FLAGS;

typedef struct tdTPM_STCLEAR_FLAGS{
	TPM_STRUCTURE_TAG tag;
	TSS_BOOL deactivated;
	TSS_BOOL disableForceClear;
	TSS_BOOL physicalPresence;
	TSS_BOOL physicalPresenceLock;
	TSS_BOOL bGlobalLock;
} TPM_STCLEAR_FLAGS;

#endif  /* VBOOT_REFERENCE_TSS_CONSTANTS_H_ */
