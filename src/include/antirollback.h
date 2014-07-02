/* Copyright (c) 2013 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * Functions for querying, manipulating and locking rollback indices
 * stored in the TPM NVRAM.
 */

#ifndef VBOOT_REFERENCE_ROLLBACK_INDEX_H_
#define VBOOT_REFERENCE_ROLLBACK_INDEX_H_

#include "sysincludes.h"
#include "tss_constants.h"

/* TPM NVRAM location indices. */
#define FIRMWARE_NV_INDEX               0x1007
#define KERNEL_NV_INDEX                 0x1008
/* This is just an opaque space for backup purposes */
#define BACKUP_NV_INDEX                 0x1009
#define BACKUP_NV_SIZE 16


/* Structure definitions for TPM spaces */

/* Kernel space - KERNEL_NV_INDEX, locked with physical presence. */
#define ROLLBACK_SPACE_KERNEL_VERSION 2
#define ROLLBACK_SPACE_KERNEL_UID 0x4752574C  /* 'GRWL' */

typedef struct RollbackSpaceKernel {
	/* Struct version, for backwards compatibility */
	uint8_t struct_version;
	/* Unique ID to detect space redefinition */
	uint32_t uid;
	/* Kernel versions */
	uint32_t kernel_versions;
	/* Reserved for future expansion */
	uint8_t reserved[3];
	/* Checksum (v2 and later only) */
	uint8_t crc8;
} __attribute__((packed)) RollbackSpaceKernel;

/* Flags for firmware space */
/*
 * Last boot was developer mode.  TPM ownership is cleared when transitioning
 * to/from developer mode.
 */
#define FLAG_LAST_BOOT_DEVELOPER 0x01
/*
 * Some systems may not have a dedicated dev-mode switch, but enter and leave
 * dev-mode through some recovery-mode magic keypresses. For those systems, the
 * dev-mode "switch" state is in this bit (0=normal, 1=dev). To make it work, a
 * new flag is passed to VbInit(), indicating that the system lacks a physical
 * dev-mode switch. If a physical switch is present, this bit is ignored.
 */
#define FLAG_VIRTUAL_DEV_MODE_ON 0x02

/* Firmware space - FIRMWARE_NV_INDEX, locked with global lock. */
#define ROLLBACK_SPACE_FIRMWARE_VERSION 2

typedef struct RollbackSpaceFirmware {
	/* Struct version, for backwards compatibility */
	uint8_t struct_version;
	/* Flags (see FLAG_* above) */
	uint8_t flags;
	/* Firmware versions */
	uint32_t fw_versions;
	/* Reserved for future expansion */
	uint8_t reserved[3];
	/* Checksum (v2 and later only) */
	uint8_t crc8;
} __attribute__((packed)) RollbackSpaceFirmware;


/* All functions return TPM_SUCCESS (zero) if successful, non-zero if error */

/*
 * These functions are called from VbInit().  They cannot use global
 * variables.
 */

uint32_t RollbackS3Resume(void);

/*
 * These functions are callable from VbSelectFirmware().  They cannot use
 * global variables.
 */

/**
 * This must be called.
 */
uint32_t RollbackFirmwareSetup(int is_hw_dev,
                               int disable_dev_request,
                               int clear_tpm_owner_request,
                               /* two outputs on success */
                               int *is_virt_dev, uint32_t *tpm_version);

/**
 * Write may be called if the versions change.
 */
uint32_t RollbackFirmwareWrite(uint32_t version);

/**
 * Lock must be called.
 */
uint32_t RollbackFirmwareLock(void);

/*
 * These functions are callable from VbSelectAndLoadKernel().  They may use
 * global variables.
 */

/**
 * Read stored kernel version.
 */
uint32_t RollbackKernelRead(uint32_t *version);

/**
 * Write stored kernel version.
 */
uint32_t RollbackKernelWrite(uint32_t version);

/**
 * Read backup data.
 */
uint32_t RollbackBackupRead(uint8_t *raw);

/**
 * Write backup data.
 */
uint32_t RollbackBackupWrite(uint8_t *raw);

/**
 * Lock must be called.  Internally, it's ignored in recovery mode.
 */
uint32_t RollbackKernelLock(int recovery_mode);

/****************************************************************************/

/*
 * The following functions are internal apis, listed here for use by unit tests
 * only.
 */

/**
 * Issue a TPM_Clear and reenable/reactivate the TPM.
 */
uint32_t TPMClearAndReenable(void);

/**
 * Like TlclWrite(), but checks for write errors due to hitting the 64-write
 * limit and clears the TPM when that happens.  This can only happen when the
 * TPM is unowned, so it is OK to clear it (and we really have no choice).
 * This is not expected to happen frequently, but it could happen.
 */
uint32_t SafeWrite(uint32_t index, const void *data, uint32_t length);

/**
 * Similarly to SafeWrite(), this ensures we don't fail a DefineSpace because
 * we hit the TPM write limit.  This is even less likely to happen than with
 * writes because we only define spaces once at initialization, but we'd rather
 * be paranoid about this.
 */
uint32_t SafeDefineSpace(uint32_t index, uint32_t perm, uint32_t size);

/**
 * Perform one-time initializations.
 *
 * Create the NVRAM spaces, and set their initial values as needed.  Sets the
 * nvLocked bit and ensures the physical presence command is enabled and
 * locked.
 */
uint32_t OneTimeInitializeTPM(RollbackSpaceFirmware *rsf,
                              RollbackSpaceKernel *rsk);

/**
 * Start the TPM and establish the root of trust for the anti-rollback
 * mechanism.
 */
uint32_t SetupTPM(int developer_mode, int disable_dev_request,
                  int clear_tpm_owner_request, RollbackSpaceFirmware *rsf);

/**
 * Utility function to turn the virtual dev-mode flag on or off. 0=off, 1=on.
 */
uint32_t SetVirtualDevMode(int val);

#endif  /* VBOOT_REFERENCE_ROLLBACK_INDEX_H_ */
