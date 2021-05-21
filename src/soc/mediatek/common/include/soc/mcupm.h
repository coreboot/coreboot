/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_COMMON_MCUPM_H
#define SOC_MEDIATEK_COMMON_MCUPM_H

#include <soc/addressmap.h>
#include <types.h>

struct mcupm_regs {
	u32 sw_rstn;
};
static struct mcupm_regs *const mcupm_reg = (void *)MCUPM_CFG_BASE;
void mcupm_init(void);
#endif  /* SOC_MEDIATEK_COMMON_MCUPM_H */
