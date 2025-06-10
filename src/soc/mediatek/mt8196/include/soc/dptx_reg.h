/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef __SOC_MEDIATEK_MT8196_DP_DPTX_REG_H__
#define __SOC_MEDIATEK_MT8196_DP_DPTX_REG_H__

#include <soc/dptx_reg_v2.h>

#define DP_PHY_DIG_TX_CTL_0				0x1474
#define RGS_AUX_LDO_EN_READY_MASK			BIT(1)
#define DRIVING_FORCE					0x30
#define EDP_TX_LN_PRE_EMPH_VAL_MASK			GENMASK(4, 3)
#define EDP_TX_LN_PRE_EMPH_VAL_SHIFT			3

#endif /* __SOC_MEDIATEK_MT8196_DP_DPTX_REG_H__ */
