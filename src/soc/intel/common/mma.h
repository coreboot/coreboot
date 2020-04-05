/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef _SOC_MMA_H_
#define _SOC_MMA_H_

#include <stdint.h>
#include <commonlib/region.h>

struct mma_config_param {
	struct region_device test_content;
	struct region_device test_param;
};

/* Locate mma metadata in CBFS, parse, find and fill rdev for
 * mma test content and test param.
 * Returns 0 on success, < 0 on failure.
 */
int mma_locate_param(struct mma_config_param *mma_cfg);
/* Locate the MMA hob from the FSP Hob list, This is implemented
 * specific to FSP version.
 * Returns 0 on success, < 0 on failure.
 */
int fsp_locate_mma_results(const void **mma_hob, size_t *mma_hob_size);

#endif
