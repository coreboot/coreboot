/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef TSS2_H_
#define TSS2_H_

#include <types.h>
#include <vb2_sha.h>

#include <security/tpm/tss/tcg-2.0/tss_structures.h>
#include <security/tpm/tss_errors.h>

/*
 * TPM2-specific
 *
 * Some operations don't have counterparts in standard and are directly exposed
 * here.
 *
 * Other operations are applicable to both TPM versions and have wrappers which
 * pick the implementation based on version determined during initialization via
 * tlcl_lib_init().
 */

/*
 * Define a TPM2 space. The define space command TPM command used by the tlcl
 * layer offers the ability to use custom nv attributes and policies.
 */
tpm_result_t tlcl2_define_space(uint32_t space_index, size_t space_size,
				const TPMA_NV nv_attributes,
				const uint8_t *nv_policy, size_t nv_policy_size);

/*
 * Issue TPM2_GetCapability command
 */
tpm_result_t tlcl2_get_capability(TPM_CAP capability, uint32_t property,
				  uint32_t property_count,
				  TPMS_CAPABILITY_DATA *capability_data);

/* Issue TPM2_NV_SetBits command */
tpm_result_t tlcl2_set_bits(uint32_t index, uint64_t bits);

/*
 * Makes tlcl2_process_command available for on top implementations of
 * custom tpm standards like cr50
 */
void *tlcl2_process_command(TPM_CC command, void *command_body);

/* Return digest size of hash algorithm */
uint16_t tlcl2_get_hash_size_from_algo(TPMI_ALG_HASH hash_algo);

/**
 * Set Clear Control. The TPM error code is returned.
 */
tpm_result_t tlcl2_clear_control(bool disable);

/**
 * Make an NV Ram location read_only.  The TPM error code is returned.
 */
tpm_result_t tlcl2_lock_nv_write(uint32_t index);

/**
 * Disable platform hierarchy. Specific to TPM2. The TPM error code is returned.
 */
tpm_result_t tlcl2_disable_platform_hierarchy(void);

/*
 * Declarations for "private" functions which are dispatched to by tss/tss.c
 * based on TPM family.
 */

tpm_result_t tlcl2_save_state(void);
tpm_result_t tlcl2_resume(void);
tpm_result_t tlcl2_startup(void);
tpm_result_t tlcl2_self_test_full(void);
tpm_result_t tlcl2_read(uint32_t index, void *data, uint32_t length);
tpm_result_t tlcl2_write(uint32_t index, const void *data, uint32_t length);
tpm_result_t tlcl2_assert_physical_presence(void);
tpm_result_t tlcl2_physical_presence_cmd_enable(void);
tpm_result_t tlcl2_finalize_physical_presence(void);
tpm_result_t tlcl2_force_clear(void);
tpm_result_t tlcl2_extend(int pcr_num, const uint8_t *digest_data,
			  enum vb2_hash_algorithm digest_algo);

#endif /* TSS2_H_ */
