/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * This file is created based on MT8188 Functional Specification
 * Chapter number: 3.7
 */

#ifndef __SOC_MEDIATEK_MT8188_DRAMC_PARAM_H__
#define __SOC_MEDIATEK_MT8188_DRAMC_PARAM_H__

/*
 * NOTE: This file is shared between coreboot and dram blob. Any change in this
 * file should be synced to the other repository.
 */

#include <soc/dramc_param_common.h>
#include <soc/dramc_soc.h>
#include <stdint.h>
#include <sys/types.h>

#define DRAMC_PARAM_HEADER_VERSION 1

struct sdram_params {
	/* Not needed for full calibration */
};

struct dramc_data {
	struct ddr_base_info ddr_info;
	struct sdram_params freq_params[DRAM_DFS_SHU_MAX];
};

struct dramc_param {
	struct dramc_param_header header;
	void (*do_putc)(unsigned char c);
	struct dramc_data dramc_datas;
};

const struct sdram_info *get_sdram_config(void);
struct dramc_param *get_dramc_param_from_blob(void *blob);
void dump_param_header(const void *blob);
int validate_dramc_param(const void *blob);
int is_valid_dramc_param(const void *blob);
int initialize_dramc_param(void *blob);

#endif  /* __SOC_MEDIATEK_MT8188_DRAMC_PARAM_H__ */
