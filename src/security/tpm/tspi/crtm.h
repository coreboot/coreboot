/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SECURITY_TSPI_CRTM_H__
#define __SECURITY_TSPI_CRTM_H__

#include <program_loading.h>
#include <security/tpm/tspi.h>
#include <types.h>
#include <vb2_sha.h>

#if CONFIG(TPM_LOG_CB) && CONFIG(TPM1)
#  define TPM_MEASURE_ALGO VB2_HASH_SHA1
#elif CONFIG(TPM_LOG_CB) && CONFIG(TPM2)
#  define TPM_MEASURE_ALGO VB2_HASH_SHA256
#elif CONFIG(TPM_LOG_TPM1)
#  define TPM_MEASURE_ALGO VB2_HASH_SHA1
#elif CONFIG(TPM_LOG_TPM2)
#  if CONFIG(TPM_HASH_SHA1)
#    define TPM_MEASURE_ALGO VB2_HASH_SHA1
#  endif
#  if CONFIG(TPM_HASH_SHA256)
#    define TPM_MEASURE_ALGO VB2_HASH_SHA256
#  endif
#  if CONFIG(TPM_HASH_SHA384)
#    define TPM_MEASURE_ALGO VB2_HASH_SHA384
#  endif
#  if CONFIG(TPM_HASH_SHA512)
#    define TPM_MEASURE_ALGO VB2_HASH_SHA512
#  endif
#endif

#if !defined(TPM_MEASURE_ALGO)
#  if !CONFIG(TPM_MEASURED_BOOT)
#    define TPM_MEASURE_ALGO VB2_HASH_INVALID
#  else
#    error "Misconfiguration: failed to determine TPM hashing algorithm"
#  endif
#endif

/**
 * Measure digests cached in TPM log entries into PCRs
 */
int tspi_measure_cache_to_pcr(void);

/**
 * Extend a measurement hash taken for a CBFS file into the appropriate PCR.
 */
uint32_t tspi_cbfs_measurement(const char *name, uint32_t type, const struct vb2_hash *hash);

/*
 * Provide a function on SoC level to measure the bootblock for cases where bootblock is
 * neither in FMAP nor in CBFS (e.g. in IFWI).
 */
int tspi_soc_measure_bootblock(int pcr_index);

#endif /* __SECURITY_TSPI_CRTM_H__ */
