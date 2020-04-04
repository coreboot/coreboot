/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __SOC_ROCKCHIP_RK3399_TSADC_H__
#define __SOC_ROCKCHIP_RK3399_TSADC_H__

#include <stdint.h>

enum {
	TSHUT_POL_HIGH = 1 << 8,
	TSHUT_POL_LOW = 0 << 8
};

void tsadc_init(uint32_t polarity);

#endif
