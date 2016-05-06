/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Rockchip Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __SOC_ROCKCHIP_RK3399_TSADC_H__
#define __SOC_ROCKCHIP_RK3399_TSADC_H__

enum {
	TSHUT_POL_HIGH = 1 << 8,
	TSHUT_POL_LOW = 0 << 8
};

void tsadc_init(uint32_t polarity);

#endif
