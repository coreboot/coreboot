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

#ifndef SOC_MEDIATEK_MT8183_SSPM_H
#define SOC_MEDIATEK_MT8183_SSPM_H

#include <soc/addressmap.h>
#include <types.h>

struct mt8183_sspm_regs {
	u32 sw_rstn;
};
static struct mt8183_sspm_regs *const mt8183_sspm = (void *)SSPM_CFG_BASE;
void sspm_init(void);
#endif  /* SOC_MEDIATEK_MT8183_SSPM_H */
