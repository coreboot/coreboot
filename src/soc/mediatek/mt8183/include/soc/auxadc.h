/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef _MTK_ADC_H
#define _MTK_ADC_H

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

/* Return voltage in uVolt */
int auxadc_get_voltage(unsigned int channel);
#endif
