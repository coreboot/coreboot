/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _MTK_EFUSE_H
#define _MTK_EFUSE_H

#include <soc/addressmap.h>
#include <types.h>

struct efuse_regs {
	uint32_t rserved[109];
	uint32_t adc_cali_reg;
};

check_member(efuse_regs, adc_cali_reg, 0x1b4);
static struct efuse_regs *const mtk_efuse = (void *)EFUSEC_BASE;

#endif
