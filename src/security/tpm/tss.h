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

#include <security/tpm/tis.h>
#include <security/tpm/tss_errors.h>
#include <security/tpm/tss/vendor/cr50/cr50.h>
#include <security/tpm/tss/tcg-1.2/tss_structures.h>
#include <security/tpm/tss/tcg-2.0/tss_structures.h>
#include <security/tpm/tss1.h>
#include <security/tpm/tss2.h>

/*
 * Operations that are applicable to both TPM versions have wrappers which
 * pick the implementation based on version determined during initialization via
 * tlcl_lib_init().
 *
 * Other operations are defined in tss1.h and tss2.h.
 */

/**
 * Call this first.  Returns 0 if success, nonzero if error.
 */
tpm_result_t tlcl_lib_init(void);

/**
 * Query active TPM family.  Returns TPM_UNKNOWN if uninitialized and TPM_1 or TPM_2 otherwise.
 */
static inline enum tpm_family tlcl_get_family(void)
{
	/* Defined in tss/tss.c */
	extern enum tpm_family tlcl_tpm_family;

	if (CONFIG(TPM1) && CONFIG(TPM2))
		return tlcl_tpm_family;
	if (CONFIG(TPM1))
		return TPM_1;
	if (CONFIG(TPM2))
		return TPM_2;
	return TPM_UNKNOWN;
}

/* Commands */

#define TLCL_CALL(name, ...) do {                         \
		if (tlcl_get_family() == TPM_1)           \
			return tlcl1_##name(__VA_ARGS__); \
		if (tlcl_get_family() == TPM_2)           \
			return tlcl2_##name(__VA_ARGS__); \
		return TPM_CB_INTERNAL_INCONSISTENCY;     \
	} while (0)

/**
 * Send a TPM_Startup(ST_CLEAR).  The TPM error code is returned (0 for
 * success).
 */
static inline tpm_result_t tlcl_startup(void)
{
	TLCL_CALL(startup);
}

/**
 * Resume by sending a TPM_Startup(ST_STATE).  The TPM error code is returned
 * (0 for success).
 */
static inline tpm_result_t tlcl_resume(void)
{
	TLCL_CALL(resume);
}

/**
 * Save TPM state by sending either TPM_SaveState() (TPM1.2) or
 * TPM_Shutdown(ST_STATE) (TPM2.0).  The TPM error code is returned (0 for
 * success).
 */
static inline tpm_result_t tlcl_save_state(void)
{
	TLCL_CALL(save_state);
}

/**
 * Run the self test.
 *
 * Note---this is synchronous.  To run this in parallel with other firmware,
 * use ContinueSelfTest().  The TPM error code is returned.
 */
static inline tpm_result_t tlcl_self_test_full(void)
{
	TLCL_CALL(self_test_full);
}

/**
 * Write [length] bytes of [data] to space at [index].  The TPM error code is
 * returned.
 */
static inline tpm_result_t tlcl_write(uint32_t index, const void *data, uint32_t length)
{
	TLCL_CALL(write, index, data, length);
}

/**
 * Read [length] bytes from space at [index] into [data].  The TPM error code
 * is returned.
 */
static inline tpm_result_t tlcl_read(uint32_t index, void *data, uint32_t length)
{
	TLCL_CALL(read, index, data, length);
}

/**
 * Assert physical presence in software.  The TPM error code is returned.
 */
static inline tpm_result_t tlcl_assert_physical_presence(void)
{
	TLCL_CALL(assert_physical_presence);
}

/**
 * Enable the physical presence command.  The TPM error code is returned.
 */
static inline tpm_result_t tlcl_physical_presence_cmd_enable(void)
{
	TLCL_CALL(physical_presence_cmd_enable);
}

/**
 * Finalize the physical presence settings: software PP is enabled, hardware PP
 * is disabled, and the lifetime lock is set.  The TPM error code is returned.
 */
static inline tpm_result_t tlcl_finalize_physical_presence(void)
{
	TLCL_CALL(finalize_physical_presence);
}

/**
 * Issue a ForceClear.  The TPM error code is returned.
 */
static inline tpm_result_t tlcl_force_clear(void)
{
	TLCL_CALL(force_clear);
}

/**
 * Perform a TPM_Extend.
 */
static inline tpm_result_t tlcl_extend(int pcr_num, const uint8_t *digest_data,
				       enum vb2_hash_algorithm digest_algo)
{
	TLCL_CALL(extend, pcr_num, digest_data, digest_algo);
}

extern tis_sendrecv_fn tlcl_tis_sendrecv;

#endif /* TSS_H_ */
