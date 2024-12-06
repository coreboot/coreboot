/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_COMMON_SSPM_H
#define SOC_MEDIATEK_COMMON_SSPM_H

#include <soc/addressmap.h>
#include <types.h>

struct sspm_regs {
	u32 sw_rstn;
};
static struct sspm_regs *const sspm_reg = (void *)SSPM_CFG_BASE;

void sspm_enable_sram(void);
void sspm_init(void);

#endif  /* SOC_MEDIATEK_COMMON_SSPM_H */
