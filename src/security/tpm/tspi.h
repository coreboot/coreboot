/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef TSPI_H_
#define TSPI_H_

#include <security/tpm/tpm1_log_serialized.h>
#include <security/tpm/tpm2_log_serialized.h>
#include <security/tpm/tspi/logs.h>
#include <security/tpm/tss.h>
#include <commonlib/tpm_log_serialized.h>
#include <commonlib/region.h>
#include <vb2_api.h>

#define TPM_PCR_MAX_LEN 64
#define HASH_DATA_CHUNK_SIZE 1024
#define MAX_TPM_LOG_ENTRIES 50
/* Assumption of 2K TCPA log size reserved for CAR/SRAM */
#define MAX_PRERAM_TPM_LOG_ENTRIES 15

/**
 * Get the pointer to the single instance of global
 * TPM log data, and initialize it when necessary
 */
void *tpm_log_init(void);

/**
 * Get the pointer to the single CBMEM instance of global
 * TPM log data, and initialize it when necessary
 */
static inline void *tpm_log_cbmem_init(void)
{
	if (CONFIG(TPM_LOG_CB))
		return tpm_cb_log_cbmem_init();
	if (CONFIG(TPM_LOG_TPM1))
		return tpm1_log_cbmem_init();
	if (CONFIG(TPM_LOG_TPM2))
		return tpm2_log_cbmem_init();
	return NULL;
}

/**
 * Clears the pre-RAM TPM log data and initializes
 * any content with default values
 */
static inline void tpm_preram_log_clear(void)
{
	if (CONFIG(TPM_LOG_CB))
		tpm_cb_preram_log_clear();
	else if (CONFIG(TPM_LOG_TPM1))
		tpm1_preram_log_clear();
	else if (CONFIG(TPM_LOG_TPM2))
		tpm2_preram_log_clear();
}

/**
 * Retrieves number of entries currently stored in the log.
 */
static inline uint16_t tpm_log_get_size(const void *log_table)
{
	if (CONFIG(TPM_LOG_CB))
		return tpm_cb_log_get_size(log_table);
	if (CONFIG(TPM_LOG_TPM1))
		return tpm1_log_get_size(log_table);
	if (CONFIG(TPM_LOG_TPM2))
		return tpm2_log_get_size(log_table);
	return 0;
}

/**
 * Copies data from pre-RAM TPM log to CBMEM (RAM) log
 */
static inline void tpm_log_copy_entries(const void *from, void *to)
{
	if (CONFIG(TPM_LOG_CB))
		tpm_cb_log_copy_entries(from, to);
	else if (CONFIG(TPM_LOG_TPM1))
		tpm1_log_copy_entries(from, to);
	else if (CONFIG(TPM_LOG_TPM2))
		tpm2_log_copy_entries(from, to);
}

/**
 * Retrieves an entry from a log. Returns non-zero on invalid index or error.
 */
static inline int tpm_log_get(int entry_idx, int *pcr, const uint8_t **digest_data,
			      enum vb2_hash_algorithm *digest_algo, const char **event_name)
{
	if (CONFIG(TPM_LOG_CB))
		return tpm_cb_log_get(entry_idx, pcr, digest_data, digest_algo, event_name);
	if (CONFIG(TPM_LOG_TPM1))
		return tpm1_log_get(entry_idx, pcr, digest_data, digest_algo, event_name);
	if (CONFIG(TPM_LOG_TPM2))
		return tpm2_log_get(entry_idx, pcr, digest_data, digest_algo, event_name);
	return 1;
}

/**
 * Add table entry for cbmem TPM log.
 * @param name Name of the hashed data
 * @param pcr PCR used to extend hashed data
 * @param diget_algo sets the digest algorithm
 * @param digest sets the hash extended into the tpm
 * @param digest_len the length of the digest
 */
static inline void tpm_log_add_table_entry(const char *name, const uint32_t pcr,
					   enum vb2_hash_algorithm digest_algo,
					   const uint8_t *digest,
					   const size_t digest_len)
{
	if (CONFIG(TPM_LOG_CB))
		tpm_cb_log_add_table_entry(name, pcr, digest_algo, digest, digest_len);
	else if (CONFIG(TPM_LOG_TPM1))
		tpm1_log_add_table_entry(name, pcr, digest_algo, digest, digest_len);
	else if (CONFIG(TPM_LOG_TPM2))
		tpm2_log_add_table_entry(name, pcr, digest_algo, digest, digest_len);
}

/**
 * Dump TPM log entries on console
 */
static inline void tpm_log_dump(void *unused)
{
	if (CONFIG(TPM_LOG_CB))
		tpm_cb_log_dump();
	else if (CONFIG(TPM_LOG_TPM1))
		tpm1_log_dump();
	else if (CONFIG(TPM_LOG_TPM2))
		tpm2_log_dump();
}

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
