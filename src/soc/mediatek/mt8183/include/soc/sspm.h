/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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
