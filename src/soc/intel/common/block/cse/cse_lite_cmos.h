/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_CSE_LITE_CMOS_H
#define SOC_INTEL_COMMON_BLOCK_CSE_LITE_CMOS_H

#include <intelblocks/cse.h>

/* Helper function to read CSE fpt information from cmos memory. */
void cmos_read_fw_partition_info(struct cse_specific_info *info);

/* Helper function to write CSE fpt information to cmos memory. */
void cmos_write_fw_partition_info(const struct cse_specific_info *info);

/* Helper function to update the `psr_backup_status` in CMOS memory */
void update_psr_backup_status(int8_t status);

/*
 * Helper function to retrieve the current `psr_backup_status` in CMOS memory
 * Returns current status on success, the status can be PSR_BACKUP_DONE or PSR_BACKUP_PENDING.
 * Returns -1 in case of signature mismatch or checksum failure.
 */
int8_t get_psr_backup_status(void);

#endif /* SOC_INTEL_COMMON_BLOCK_CSE_LITE_CMOS_H */
