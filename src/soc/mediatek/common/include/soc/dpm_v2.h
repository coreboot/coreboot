/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MEDIATEK_COMMON_DPM_V2_H__
#define __SOC_MEDIATEK_COMMON_DPM_V2_H__

#include <soc/addressmap.h>
#include <soc/dpm_common.h>

#define DPM_RST_OFFSET		0x7074
#define DPM_SW_RSTN		BIT(0)

#define DPM_CFG_CH0		DPM_CFG_BASE
#define DPM_BARGS_CH0_REG0	(DPM_CFG_BASE + 0x6004)
#define DPM_BARGS_CH0_REG1	(DPM_CFG_BASE + 0x6008)
#define DRAMC_WBR		(INFRACFG_AO_BASE + 0x0b4)

#define ENABLE_DRAMC_WBR_MASK	0x2ffff

#endif  /* __SOC_MEDIATEK_COMMON_DPM_V2_H__ */
