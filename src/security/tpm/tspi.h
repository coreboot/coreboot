/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef TSPI_H_
#define TSPI_H_

#include <security/tpm/tss.h>
#include <commonlib/tpm_log_serialized.h>
#include <commonlib/region.h>
#include <vb2_api.h>

#define TPM_PCR_MAX_LEN 64
#define HASH_DATA_CHUNK_SIZE 1024

/**
 * Get the pointer to the single instance of global
 * TPM log data, and initialize it when necessary
 */
struct tpm_cb_log_table *tpm_log_init(void);

/**
 * Clears the pre-RAM TPM log data and initializes
 * any content with default values
 */
void tpm_preram_log_clear(void);

/**
 * Add table entry for cbmem TPM log.
 * @param name Name of the hashed data
 * @param pcr PCR used to extend hashed data
 * @param diget_algo sets the digest algorithm
 * @param digest sets the hash extended into the tpm
 * @param digest_len the length of the digest
 */
void tpm_log_add_table_entry(const char *name, const uint32_t pcr,
			     enum vb2_hash_algorithm digest_algo,
			     const uint8_t *digest,
			     const size_t digest_len);

/**
 * Dump TPM log entries on console
 */
void tpm_log_dump(void *unused);

/**
 * Ask vboot for a digest and extend a TPM PCR with it.
 * @param pcr sets the pcr index
 * @param diget_algo sets the digest algorithm
 * @param digest sets the hash to extend into the tpm
 * @param digest_len the length of the digest
 * @param name sets additional info where the digest comes from
 * @return TPM_SUCCESS on success. If not a tpm error is returned
 */
uint32_t tpm_extend_pcr(int pcr, enum vb2_hash_algorithm digest_algo,
			const uint8_t *digest, size_t digest_len,
			const char *name);

/**
 * Issue a TPM_Clear and re-enable/reactivate the TPM.
 * @return TPM_SUCCESS on success. If not a tpm error is returned
 */
uint32_t tpm_clear_and_reenable(void);

/**
 * Start the TPM and establish the root of trust.
 * @param s3flag tells the tpm setup if we wake up from a s3 state on x86
 * @return TPM_SUCCESS on success. If not a tpm error is returned
 */
uint32_t tpm_setup(int s3flag);

/**
 * Measure a given region device and extend given PCR with the result.
 * @param *rdev Pointer to the region device to measure
 * @param pcr Index of the PCR which will be extended by this measure
 * @param *rname Name of the region that is measured
 * @return TPM error code in case of error otherwise TPM_SUCCESS
 */
uint32_t tpm_measure_region(const struct region_device *rdev, uint8_t pcr,
			    const char *rname);

#endif /* TSPI_H_ */
