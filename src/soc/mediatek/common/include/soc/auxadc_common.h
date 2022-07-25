/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _MTK_ADC_COMMON_H
#define _MTK_ADC_COMMON_H

struct mtk_auxadc_regs {
	uint32_t con0;
	uint32_t con1;
	uint32_t con1_set;
	uint32_t con1_clr;
	uint32_t con2;
	uint32_t data[16];
	uint32_t reserved[16];
	uint32_t misc;
};

/* Return voltage in uVolt */
unsigned int auxadc_get_voltage_uv(unsigned int channel);
#endif
