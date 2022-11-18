/* SPDX-License-Identifier: BSD-3-Clause */

/*
 * Some TPM constants and type definitions for standalone compilation for use
 * in the firmware
 */
#ifndef TCG1_TSS_STRUCTURES_H_
#define TCG1_TSS_STRUCTURES_H_

#include <stdint.h>
#include "../common/tss_common.h"

#define TPM_MAX_COMMAND_SIZE 4096
#define TPM_LARGE_ENOUGH_COMMAND_SIZE 256  /* saves space in the firmware */
#define TPM_PUBEK_SIZE 256

#define TPM_NV_INDEX0 ((uint32_t)0x00000000)
#define TPM_NV_INDEX_LOCK ((uint32_t)0xffffffff)
#define TPM_NV_PER_GLOBALLOCK (((uint32_t)1)<<15)
#define TPM_NV_PER_PPWRITE (((uint32_t)1)<<0)
#define TPM_NV_PER_READ_STCLEAR (((uint32_t)1)<<31)
#define TPM_NV_PER_WRITE_STCLEAR (((uint32_t)1)<<14)

#define TPM_TAG_RQU_COMMAND       ((uint16_t)0xc1)
#define TPM_TAG_RQU_AUTH1_COMMAND ((uint16_t)0xc2)
#define TPM_TAG_RQU_AUTH2_COMMAND ((uint16_t)0xc3)

#define TPM_TAG_RSP_COMMAND       ((uint16_t)0xc4)
#define TPM_TAG_RSP_AUTH1_COMMAND ((uint16_t)0xc5)
#define TPM_TAG_RSP_AUTH2_COMMAND ((uint16_t)0xc6)

typedef uint8_t TSS_BOOL;
typedef uint16_t TPM_STRUCTURE_TAG;

typedef struct tdTPM_PERMANENT_FLAGS {
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

typedef struct tdTPM_STCLEAR_FLAGS {
	TPM_STRUCTURE_TAG tag;
	TSS_BOOL deactivated;
	TSS_BOOL disableForceClear;
	TSS_BOOL physicalPresence;
	TSS_BOOL physicalPresenceLock;
	TSS_BOOL bGlobalLock;
} TPM_STCLEAR_FLAGS;

#endif  /* TCG1_TSS_STRUCTURES_H_ */
