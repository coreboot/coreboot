/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SECURITY_TSPI_CRTM_H__
#define __SECURITY_TSPI_CRTM_H__

#include <commonlib/cbfs.h>
#include <program_loading.h>
#include <security/tpm/tspi.h>
#include <types.h>

/* CRTM */
#define TPM_CRTM_PCR 2

/* PCR for measuring data which changes during runtime
 * e.g. CMOS, NVRAM...
 */
#define TPM_RUNTIME_DATA_PCR 3

/**
 * Measure digests cached in TCPA log entries into PCRs
 */
int tspi_measure_cache_to_pcr(void);

#if !ENV_SMM && CONFIG(TPM_MEASURED_BOOT)
/*
 * Measures cbfs data via hook (cbfs)
 * rdev covers the file data (not metadata)
 * return 0 if successful, else an error
 */
uint32_t tspi_measure_cbfs_hook(const struct region_device *rdev,
				const char *name, uint32_t cbfs_type);
#else
#define tspi_measure_cbfs_hook(rdev, name, cbfs_type) 0
#endif

#endif /* __SECURITY_TSPI_CRTM_H__ */
