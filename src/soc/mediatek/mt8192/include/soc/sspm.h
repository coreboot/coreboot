/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_MT8192_SSPM_H
#define SOC_MEDIATEK_MT8192_SSPM_H

#include <soc/addressmap.h>
#include <types.h>

struct mt8192_sspm_regs {
	u32 sw_rstn;
};
static struct mt8192_sspm_regs *const mt8192_sspm = (void *)SSPM_CFG_BASE;
void sspm_init(void);
#endif  /* SOC_MEDIATEK_MT8192_SSPM_H */
