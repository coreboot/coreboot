/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
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

#include <soc/common.h>
#include <soc/clock.h>

uint32_t mv_tclk_get(void)
{
	uint32_t tclk_reg_value;
	uint32_t tclk;

	tclk_reg_value = (mrvl_reg_read(MPP_SAMPLE_AT_RESET));
	tclk_reg_value = ((tclk_reg_value & (1 << 15)) >> 15);
	switch (tclk_reg_value) {
	case 0:
		tclk = MV_BOARD_TCLK_250MHZ;
	case 1:
		tclk = MV_BOARD_TCLK_200MHZ;
	default:
		tclk = MV_BOARD_TCLK_250MHZ;
	}
	return tclk;
}
