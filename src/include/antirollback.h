/* Copyright (c) 2013 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * Functions for querying, manipulating and locking rollback indices
 * stored in the TPM NVRAM.
 */

#ifndef ANTIROLLBACK_H_
#define ANTIROLLBACK_H_

#include "2sysincludes.h"
#include <2api.h>
#include "tpm_lite/tss_constants.h"

/* TPM NVRAM location indices. */
#define FIRMWARE_NV_INDEX               0x1007
#define KERNEL_NV_INDEX                 0x1008
/* This is just an opaque space for backup purposes */
#define BACKUP_NV_INDEX                 0x1009

/* Structure definitions for TPM spaces */

/* Flags for firmware space */

/*
 * Last boot was developer mode.  TPM ownership is cleared when transitioning
 * to/from developer mode.
 */
#define FLAG_LAST_BOOT_DEVELOPER 0x01

/* All functions return TPM_SUCCESS (zero) if successful, non-zero if error */

uint32_t antirollback_read_space_firmware(struct vb2_context *ctx);

/**
 * Write may be called if the versions change.
 */
uint32_t antirollback_write_space_firmware(struct vb2_context *ctx);

/**
 * Lock must be called.
 */
uint32_t antirollback_lock_space_firmware(void);

/****************************************************************************/

/*
 * The following functions are internal apis, listed here for use by unit tests
 * only.
 */

/**
 * Issue a TPM_Clear and reenable/reactivate the TPM.
 */
uint32_t tpm_clear_and_reenable(void);

/**
 * Like tlcl_write(), but checks for write errors due to hitting the 64-write
 * limit and clears the TPM when that happens.  This can only happen when the
 * TPM is unowned, so it is OK to clear it (and we really have no choice).
 * This is not expected to happen frequently, but it could happen.
 */
uint32_t safe_write(uint32_t index, const void *data, uint32_t length);

/**
 * Similarly to safe_write(), this ensures we don't fail a DefineSpace because
 * we hit the TPM write limit.  This is even less likely to happen than with
 * writes because we only define spaces once at initialization, but we'd rather
 * be paranoid about this.
 */
uint32_t safe_define_space(uint32_t index, uint32_t perm, uint32_t size);

/**
 * Perform one-time initializations.
 *
 * Create the NVRAM spaces, and set their initial values as needed.  Sets the
 * nvLocked bit and ensures the physical presence command is enabled and
 * locked.
 */
uint32_t factory_initialize_tpm(struct vb2_context *ctx);

/**
 * Start the TPM and establish the root of trust for the antirollback mechanism.
 */
uint32_t setup_tpm(struct vb2_context *ctx);

#endif  /* ANTIROLLBACK_H_ */
