/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_MT8192_MCUPM_H
#define SOC_MEDIATEK_MT8192_MCUPM_H

#include <soc/addressmap.h>
#include <types.h>

struct mt8192_mcupm_regs {
	u32 sw_rstn;
};
static struct mt8192_mcupm_regs *const mt8192_mcupm = (void *)MCUPM_CFG_BASE;
void mcupm_init(void);
#endif  /* SOC_MEDIATEK_MT8192_MCUPM_H */
