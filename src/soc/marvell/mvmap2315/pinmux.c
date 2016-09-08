/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Marvell, Inc.
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

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <arch/io.h>
#include <soc/pinmux.h>

void set_pinmux(struct mvmap2315_pinmux pinmux)
{
	u32 pad_num;
	u32 reg = 0;

	/* pads < 160 are part of the MCU domain and not handled here
	 * and pads > 231 don't exist
	 */
	if (pinmux.pad < 160 || pinmux.pad > 231)
		return;

	pad_num = pinmux.pad - 160;

	reg |= (pinmux.fun_sel <<
		MVMAP2315_PADWRAP_FUNC_SEL_SHIFT) &
		MVMAP2315_PADWRAP_FUNC_SEL;

	reg |= (pinmux.raw_sel <<
		MVMAP2315_PADWRAP_RAW_SEL_SHIFT) &
		MVMAP2315_PADWRAP_RAW_SEL;

	reg |= (pinmux.dgtb_sel <<
		MVMAP2315_PADWRAP_DGTB_SEL_SHIFT) &
		MVMAP2315_PADWRAP_DGTB_SEL;

	reg |= (pinmux.slew <<
		MVMAP2315_PADWRAP_SLEW_SHIFT) &
		MVMAP2315_PADWRAP_SLEW;

	if (!pinmux.pull_sel) {
		reg &= ~MVMAP2315_PADWRAP_PD_EN;
		reg &= ~MVMAP2315_PADWRAP_PU_EN;
	} else if (pinmux.pull_sel == 1) {
		reg |= MVMAP2315_PADWRAP_PD_EN;
		reg &= ~MVMAP2315_PADWRAP_PU_EN;
	} else if (pinmux.pull_sel == 2) {
		reg &= ~MVMAP2315_PADWRAP_PD_EN;
		reg |= MVMAP2315_PADWRAP_PU_EN;
	}

	write32(&mvmap2315_pinmux->io_pad_piocfg[pad_num], reg);
}
