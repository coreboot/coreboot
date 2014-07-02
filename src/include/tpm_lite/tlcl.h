/* Copyright (c) 2013 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * TPM Lightweight Command Library.
 *
 * A low-level library for interfacing to TPM hardware or an emulator.
 */

#ifndef TPM_LITE_TLCL_H_
#define TPM_LITE_TLCL_H_
#include <stdint.h>

#include "tss_constants.h"

/*****************************************************************************/
/* Functions implemented in tlcl.c */

/**
 * Call this first.  Returns 0 if success, nonzero if error.
 */
uint32_t TlclLibInit(void);

/**
 * Call this on shutdown.  Returns 0 if success, nonzero if error.
 */
uint32_t TlclLibClose(void);

/* Low-level operations */

/**
 * Perform a raw TPM request/response transaction.
 */
uint32_t TlclSendReceive(const uint8_t *request, uint8_t *response,
                         int max_length);

/**
 * Return the size of a TPM request or response packet.
 */
int TlclPacketSize(const uint8_t *packet);

/* Commands */

/**
 * Send a TPM_Startup(ST_CLEAR).  The TPM error code is returned (0 for
 * success).
 */
uint32_t TlclStartup(void);

/**
 * Save the TPM state.  Normally done by the kernel before a suspend, included
 * here for tests.  The TPM error code is returned (0 for success).
 */
uint32_t TlclSaveState(void);

/**
 * Resume by sending a TPM_Startup(ST_STATE).  The TPM error code is returned
 * (0 for success).
 */
uint32_t TlclResume(void);

/**
 * Run the self test.
 *
 * Note---this is synchronous.  To run this in parallel with other firmware,
 * use ContinueSelfTest().  The TPM error code is returned.
 */
uint32_t TlclSelfTestFull(void);

/**
 * Run the self test in the background.
 */
uint32_t TlclContinueSelfTest(void);

/**
 * Define a space with permission [perm].  [index] is the index for the space,
 * [size] the usable data size.  The TPM error code is returned.
 */
uint32_t TlclDefineSpace(uint32_t index, uint32_t perm, uint32_t size);

/**
 * Write [length] bytes of [data] to space at [index].  The TPM error code is
 * returned.
 */
uint32_t TlclWrite(uint32_t index, const void *data, uint32_t length);

/**
 * Read [length] bytes from space at [index] into [data].  The TPM error code
 * is returned.
 */
uint32_t TlclRead(uint32_t index, void *data, uint32_t length);

/**
 * Read PCR at [index] into [data].  [length] must be TPM_PCR_DIGEST or
 * larger. The TPM error code is returned.
 */
uint32_t TlclPCRRead(uint32_t index, void *data, uint32_t length);

/**
 * Write-lock space at [index].  The TPM error code is returned.
 */
uint32_t TlclWriteLock(uint32_t index);

/**
 * Read-lock space at [index].  The TPM error code is returned.
 */
uint32_t TlclReadLock(uint32_t index);

/**
 * Assert physical presence in software.  The TPM error code is returned.
 */
uint32_t TlclAssertPhysicalPresence(void);

/**
 * Enable the physical presence command.  The TPM error code is returned.
 */
uint32_t TlclPhysicalPresenceCMDEnable(void);

/**
 * Finalize the physical presence settings: sofware PP is enabled, hardware PP
 * is disabled, and the lifetime lock is set.  The TPM error code is returned.
 */
uint32_t TlclFinalizePhysicalPresence(void);

uint32_t TlclAssertPhysicalPresenceResult(void);

/**
 * Turn off physical presence and locks it off until next reboot.  The TPM
 * error code is returned.
 */
uint32_t TlclLockPhysicalPresence(void);

/**
 * Set the nvLocked bit.  The TPM error code is returned.
 */
uint32_t TlclSetNvLocked(void);

/**
 * Return 1 if the TPM is owned, 0 otherwise.
 */
int TlclIsOwned(void);

/**
 * Issue a ForceClear.  The TPM error code is returned.
 */
uint32_t TlclForceClear(void);

/**
 * Issue a PhysicalEnable.  The TPM error code is returned.
 */
uint32_t TlclSetEnable(void);

/**
 * Issue a PhysicalDisable.  The TPM error code is returned.
 */
uint32_t TlclClearEnable(void);

/**
 * Issue a SetDeactivated.  Pass 0 to activate.  Returns result code.
 */
uint32_t TlclSetDeactivated(uint8_t flag);

/**
 * Get flags of interest.  Pointers for flags you aren't interested in may
 * be NULL.  The TPM error code is returned.
 */
uint32_t TlclGetFlags(uint8_t *disable, uint8_t *deactivated,
                      uint8_t *nvlocked);

/**
 * Set the bGlobalLock flag, which only a reboot can clear.  The TPM error
 * code is returned.
 */
uint32_t TlclSetGlobalLock(void);

/**
 * Perform a TPM_Extend.
 */
uint32_t TlclExtend(int pcr_num, const uint8_t *in_digest, uint8_t *out_digest);

/**
 * Get the permission bits for the NVRAM space with |index|.
 */
uint32_t TlclGetPermissions(uint32_t index, uint32_t *permissions);

/**
 * Get the entire set of permanent flags.
 */
uint32_t TlclGetPermanentFlags(TPM_PERMANENT_FLAGS *pflags);

/**
 * Get the entire set of volatile (ST_CLEAR) flags.
 */
uint32_t TlclGetSTClearFlags(TPM_STCLEAR_FLAGS *pflags);

/**
 * Get the ownership flag. The TPM error code is returned.
 */
uint32_t TlclGetOwnership(uint8_t *owned);

/**
 * Request [length] bytes from TPM RNG to be stored in [data]. Actual number of
 * bytes read is stored in [size]. The TPM error code is returned.
 */
uint32_t TlclGetRandom(uint8_t *data, uint32_t length, uint32_t *size);

#endif  /* TPM_LITE_TLCL_H_ */
