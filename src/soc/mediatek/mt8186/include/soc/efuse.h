/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_MT8186_EFUSE_H
#define SOC_MEDIATEK_MT8186_EFUSE_H

#include <soc/addressmap.h>
#include <types.h>

struct efuse_regs {
	uint32_t reserved[130];
	uint32_t adc_cali_reg;
	uint32_t reserved1[357];
	uint32_t cpu_id_reg;
};

check_member(efuse_regs, adc_cali_reg, 0x208);
check_member(efuse_regs, cpu_id_reg, 0x7a0);
static struct efuse_regs *const mtk_efuse = (void *)EFUSEC_BASE;

#endif
