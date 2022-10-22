/* SPDX-License-Identifier: BSD-3-Clause */

/*
 * TPM Lightweight Command Library.
 *
 * A low-level library for interfacing to TPM hardware or an emulator.
 */

#ifndef TSS_H_
#define TSS_H_

#include <types.h>
#include <vb2_sha.h>

#include <security/tpm/tss/common/tss_common.h>
#include <security/tpm/tss_errors.h>
#include <security/tpm/tss/vendor/cr50/cr50.h>

#if CONFIG(TPM1)

#include <security/tpm/tss/tcg-1.2/tss_structures.h>

/**
 * Define a space with permission [perm]. [index] is the index for the space,
 * [size] the usable data size. The TPM error code is returned.
 */
uint32_t tlcl_define_space(uint32_t index, uint32_t perm, uint32_t size);

/**
 * Issue a PhysicalEnable. The TPM error code is returned.
 */
uint32_t tlcl_set_enable(void);

/**
 * Issue a SetDeactivated. Pass 0 to activate. Returns result code.
 */
uint32_t tlcl_set_deactivated(uint8_t flag);

/**
 * Get flags of interest. Pointers for flags you aren't interested in may
 * be NULL. The TPM error code is returned.
 */
uint32_t tlcl_get_flags(uint8_t *disable, uint8_t *deactivated,
			uint8_t *nvlocked);

/**
 * Get the entire set of permanent flags.
 */
uint32_t tlcl_get_permanent_flags(TPM_PERMANENT_FLAGS *pflags);

#endif

#if CONFIG(TPM2)

#include <security/tpm/tss/tcg-2.0/tss_structures.h>

/*
 * Define a TPM2 space. The define space command TPM command used by the tlcl
 * layer offers the ability to use custom nv attributes and policies.
 */
uint32_t tlcl_define_space(uint32_t space_index, size_t space_size,
			   const TPMA_NV nv_attributes,
			   const uint8_t *nv_policy, size_t nv_policy_size);

/*
 * Issue TPM2_GetCapability command
 */
uint32_t tlcl_get_capability(TPM_CAP capability, uint32_t property,
			     uint32_t property_count,
			     TPMS_CAPABILITY_DATA *capability_data);

/* Issue TPM2_NV_SetBits command */
uint32_t tlcl_set_bits(uint32_t index, uint64_t bits);

/*
 * Makes tpm_process_command available for on top implementations of
 * custom tpm standards like cr50
 */
void *tpm_process_command(TPM_CC command, void *command_body);

/* Return digest size of hash algorithm */
uint16_t tlcl_get_hash_size_from_algo(TPMI_ALG_HASH hash_algo);

#endif

/*****************************************************************************/
/* Generic Functions implemented in tlcl.c */

/**
 * Call this first.  Returns 0 if success, nonzero if error.
 */
uint32_t tlcl_lib_init(void);

/**
 * Perform a raw TPM request/response transaction.
 */
uint32_t tlcl_send_receive(const uint8_t *request, uint8_t *response,
			   int max_length);

/* Commands */

/**
 * Send a TPM_Startup(ST_CLEAR).  The TPM error code is returned (0 for
 * success).
 */
uint32_t tlcl_startup(void);

/**
 * Resume by sending a TPM_Startup(ST_STATE).  The TPM error code is returned
 * (0 for success).
 */
uint32_t tlcl_resume(void);

/**
 * Save TPM state by sending either TPM_SaveState() (TPM1.2) or
 * TPM_Shutdown(ST_STATE) (TPM2.0).  The TPM error code is returned (0 for
 * success).
 */
uint32_t tlcl_save_state(void);

/**
 * Run the self test.
 *
 * Note---this is synchronous.  To run this in parallel with other firmware,
 * use ContinueSelfTest().  The TPM error code is returned.
 */
uint32_t tlcl_self_test_full(void);

/**
 * Run the self test in the background.
 */
uint32_t tlcl_continue_self_test(void);

/**
 * Write [length] bytes of [data] to space at [index].  The TPM error code is
 * returned.
 */
uint32_t tlcl_write(uint32_t index, const void *data, uint32_t length);

/**
 * Read [length] bytes from space at [index] into [data].  The TPM error code
 * is returned.
 */
uint32_t tlcl_read(uint32_t index, void *data, uint32_t length);

/**
 * Assert physical presence in software.  The TPM error code is returned.
 */
uint32_t tlcl_assert_physical_presence(void);

/**
 * Enable the physical presence command.  The TPM error code is returned.
 */
uint32_t tlcl_physical_presence_cmd_enable(void);

/**
 * Finalize the physical presence settings: software PP is enabled, hardware PP
 * is disabled, and the lifetime lock is set.  The TPM error code is returned.
 */
uint32_t tlcl_finalize_physical_presence(void);

/**
 * Set the nvLocked bit.  The TPM error code is returned.
 */
uint32_t tlcl_set_nv_locked(void);

/**
 * Issue a ForceClear.  The TPM error code is returned.
 */
uint32_t tlcl_force_clear(void);

/**
 * Set Clear Control. The TPM error code is returned.
 */
uint32_t tlcl_clear_control(bool disable);

/**
 * Set the bGlobalLock flag, which only a reboot can clear.  The TPM error
 * code is returned.
 */
uint32_t tlcl_set_global_lock(void);

/**
 * Make an NV Ram location read_only.  The TPM error code is returned.
 */
uint32_t tlcl_lock_nv_write(uint32_t index);

/**
 * Perform a TPM_Extend.
 */
uint32_t tlcl_extend(int pcr_num, const uint8_t *digest_data,
		     enum vb2_hash_algorithm digest_algo);

/**
 * Disable platform hierarchy. Specific to TPM2. The TPM error code is returned.
 */
uint32_t tlcl_disable_platform_hierarchy(void);

/**
 * Get the permission bits for the NVRAM space with |index|.
 */
uint32_t tlcl_get_permissions(uint32_t index, uint32_t *permissions);

#endif /* TSS_H_ */
