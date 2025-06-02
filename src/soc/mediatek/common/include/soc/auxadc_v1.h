/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MEDIATEK_COMMON_INCLUDE_SOC_AUXADC_V1_H__
#define __SOC_MEDIATEK_COMMON_INCLUDE_SOC_AUXADC_V1_H__

#define ADC_GE_A_SHIFT		10
#define ADC_GE_A_MASK		(0x3ff << ADC_GE_A_SHIFT)
#define ADC_OE_A_SHIFT		0
#define ADC_OE_A_MASK		(0x3ff << ADC_OE_A_SHIFT)

#define AUXADC_SWCG_CLR_REG	(mtk_infracfg->module_sw_cg_1_clr)
#define AUXADC_SWCG_SET_REG	(mtk_infracfg->module_sw_cg_1_set)
#define AUXADC_SWCG_EN_BIT	10

#endif
