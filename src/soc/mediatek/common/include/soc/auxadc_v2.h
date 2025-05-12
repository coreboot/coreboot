/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MEDIATEK_COMMON_INCLUDE_SOC_AUXADC_V2_H__
#define __SOC_MEDIATEK_COMMON_INCLUDE_SOC_AUXADC_V2_H__

#define ADC_GE_A_SHIFT		0
#define ADC_GE_A_MASK		(0x3ff << ADC_GE_A_SHIFT)
#define ADC_OE_A_SHIFT		10
#define ADC_OE_A_MASK		(0x3ff << ADC_OE_A_SHIFT)

#define AUXADC_SWCG_CLR_REG	(mtk_pericfg_ao->pericfg_ao_peri_cg_2_clr)
#define AUXADC_SWCG_SET_REG	(mtk_pericfg_ao->pericfg_ao_peri_cg_2_set)
#define AUXADC_SWCG_EN_BIT	27

#endif
