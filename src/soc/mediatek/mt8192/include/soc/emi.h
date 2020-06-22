/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_MT8192_EMI_H
#define SOC_MEDIATEK_MT8192_EMI_H

#include <soc/dramc_param.h>

size_t sdram_size(void);
void mt_set_emi(const struct dramc_data *dparam);
void mt_mem_init(struct dramc_param_ops *dparam_ops);
int complex_mem_test(u8 *start, unsigned int len);

#endif /* SOC_MEDIATEK_MT8192_EMI_H */
