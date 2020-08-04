/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _MTK_ADC_H
#define _MTK_ADC_H

#include <soc/auxadc_common.h>
#include <soc/infracfg.h>
#include <stdint.h>

typedef struct mtk_auxadc_regs {
	uint32_t con0;
	uint32_t con1;
	uint32_t con1_set;
	uint32_t con1_clr;
	uint32_t con2;
	uint32_t data[16];
	uint32_t reserved[16];
	uint32_t misc;
} mtk_auxadc_regs;

static struct mt8183_infracfg_regs *const mtk_infracfg = mt8183_infracfg;

#endif
