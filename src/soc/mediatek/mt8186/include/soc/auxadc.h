/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8186 Functional Specification
 * Chapter number: 5.9
 */

#ifndef SOC_MEDIATEK_MT8186_AUXADC_H
#define SOC_MEDIATEK_MT8186_AUXADC_H

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

static struct mt8186_infracfg_ao_regs *const mtk_infracfg = mt8186_infracfg_ao;

#endif
