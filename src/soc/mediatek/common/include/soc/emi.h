/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_EMI_H
#define SOC_MEDIATEK_EMI_H

#include <soc/dramc_param.h>

size_t sdram_size(void);
void mt_set_emi(struct dramc_param *dparam);
void mt_mem_init(struct dramc_param_ops *dparam_ops);
int complex_mem_test(u8 *start, unsigned int len);

bool is_dvfs_enabled(void);
u32 get_ddr_geometry(void);
u32 get_ddr_type(void);
void init_dram_by_params(struct dramc_param *dparam);

#endif /* SOC_MEDIATEK_MT8192_EMI_H */
