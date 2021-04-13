/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_MMA_H_
#define _SOC_MMA_H_

#include <types.h>

struct mma_config_param {
	void *test_content;
	size_t test_content_size;
	void *test_param;
	size_t test_param_size;
};

/* Locate mma metadata in CBFS, parse, find and map mma test content and test param.
 * Returns 0 on success, < 0 on failure.
 */
int mma_map_param(struct mma_config_param *mma_cfg);
/* Locate the MMA hob from the FSP Hob list, This is implemented
 * specific to FSP version.
 * Returns 0 on success, < 0 on failure.
 */
int fsp_locate_mma_results(const void **mma_hob, size_t *mma_hob_size);

#endif
