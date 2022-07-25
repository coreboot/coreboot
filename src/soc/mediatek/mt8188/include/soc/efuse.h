/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_MT8188_EFUSE_H
#define SOC_MEDIATEK_MT8188_EFUSE_H

#include <soc/addressmap.h>
#include <types.h>

struct efuse_regs {
	uint32_t reserved[107];
	uint32_t adc_cali_reg;
};

check_member(efuse_regs, adc_cali_reg, 0x1AC);
static struct efuse_regs *const mtk_efuse = (void *)EFUSE_BASE;

#endif
