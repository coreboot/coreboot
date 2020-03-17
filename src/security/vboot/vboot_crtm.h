/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __SECURITY_VBOOT_CRTM_H__
#define __SECURITY_VBOOT_CRTM_H__

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

/*
 * Initializes the Core Root of Trust for Measurements
 * in coreboot. The initial code in a chain of trust must measure
 * itself.
 *
 * Summary:
 *  + Measures bootblock in CBFS or BOOTBLOCK FMAP partition.
 *  + If vboot starts in romstage, it measures the romstage
 *    in CBFS.
 *  + Measure the verstage if it is compiled as separate
 *    stage.
 *
 * Takes the current vboot context as parameter for s3 checks.
 * returns on success VB2_SUCCESS, else a vboot error.
 */
uint32_t vboot_init_crtm(void);

#if CONFIG(VBOOT_MEASURED_BOOT)
/*
 * Measures cbfs data via hook (cbfs)
 * fh is the cbfs file handle to measure
 * return 0 if successful, else an error
 */
uint32_t vboot_measure_cbfs_hook(struct cbfsf *fh, const char *name);

#else
#define vboot_measure_cbfs_hook(fh, name) 0
#endif

#endif /* __VBOOT_VBOOT_CRTM_H__ */
