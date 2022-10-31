/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef TSS1_H_
#define TSS1_H_

#include <types.h>
#include <vb2_sha.h>

#include <security/tpm/tss/tcg-1.2/tss_structures.h>
#include <security/tpm/tss_errors.h>

/*
 * TPM1.2-specific
 *
 * Some operations don't have counterparts in standard and are directly exposed
 * here.
 *
 * Other operations are applicable to both TPM versions and have wrappers which
 * pick the implementation based on version determined during initialization via
 * tlcl_lib_init().
 */

/**
 * Define a space with permission [perm]. [index] is the index for the space,
 * [size] the usable data size. The TPM error code is returned.
 */
tpm_result_t tlcl1_define_space(uint32_t index, uint32_t perm, uint32_t size);

/**
 * Issue a PhysicalEnable. The TPM error code is returned.
 */
tpm_result_t tlcl1_set_enable(void);

/**
 * Issue a SetDeactivated. Pass 0 to activate. Returns result code.
 */
tpm_result_t tlcl1_set_deactivated(uint8_t flag);

/**
 * Get flags of interest. Pointers for flags you aren't interested in may
 * be NULL. The TPM error code is returned.
 */
tpm_result_t tlcl1_get_flags(uint8_t *disable, uint8_t *deactivated, uint8_t *nvlocked);

/**
 * Perform a raw TPM request/response transaction.
 */
tpm_result_t tlcl1_send_receive(const uint8_t *request, uint8_t *response, int max_length);

/**
 * Run the self test in the background.
 */
tpm_result_t tlcl1_continue_self_test(void);

/**
 * Set the nvLocked bit.  The TPM error code is returned.
 */
tpm_result_t tlcl1_set_nv_locked(void);

/**
 * Get the entire set of permanent flags.
 */
tpm_result_t tlcl1_get_permanent_flags(TPM_PERMANENT_FLAGS *pflags);

/**
 * Set the bGlobalLock flag, which only a reboot can clear.  The TPM error
 * code is returned.
 */
tpm_result_t tlcl1_set_global_lock(void);

/**
 * Get the permission bits for the NVRAM space with |index|.
 */
tpm_result_t tlcl1_get_permissions(uint32_t index, uint32_t *permissions);

/*
 * Declarations for "private" functions which are dispatched to by tss/tss.c
 * based on TPM family.
 */

tpm_result_t tlcl1_save_state(void);
tpm_result_t tlcl1_resume(void);
tpm_result_t tlcl1_startup(void);
tpm_result_t tlcl1_self_test_full(void);
tpm_result_t tlcl1_read(uint32_t index, void *data, uint32_t length);
tpm_result_t tlcl1_write(uint32_t index, const void *data, uint32_t length);
tpm_result_t tlcl1_assert_physical_presence(void);
tpm_result_t tlcl1_physical_presence_cmd_enable(void);
tpm_result_t tlcl1_finalize_physical_presence(void);
tpm_result_t tlcl1_force_clear(void);
tpm_result_t tlcl1_extend(int pcr_num, const uint8_t *digest_data,
			  enum vb2_hash_algorithm digest_algo);

#endif /* TSS1_H_ */
