/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

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
