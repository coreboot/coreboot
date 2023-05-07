/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_CSE_LITE_CMOS_H
#define SOC_INTEL_COMMON_BLOCK_CSE_LITE_CMOS_H

#include <intelblocks/cse.h>

/* Helper function to read CSE fpt information from cmos memory. */
void cmos_read_fw_partition_info(struct cse_specific_info *info);

/* Helper function to write CSE fpt information to cmos memory. */
void cmos_write_fw_partition_info(const struct cse_specific_info *info);

#endif /* SOC_INTEL_COMMON_BLOCK_CSE_LITE_CMOS_H */
