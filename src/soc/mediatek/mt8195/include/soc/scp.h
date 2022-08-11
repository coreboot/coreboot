/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef SOC_MEDIATEK_MT8195_SCP_H
#define SOC_MEDIATEK_MT8195_SCP_H

#include <soc/addressmap.h>

#define SCP_SRAM_PDN_DISABLE_VAL	0xFFFFFFFF

#define REG_L1TCM_SRAM_PDN		(void *)(SCP_CFG_BASE + 0x2102C)

void scp_rsi_enable(void);
void scp_rsi_disable(void);
#endif
