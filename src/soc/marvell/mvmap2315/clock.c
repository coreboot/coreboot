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

#include <arch/clock.h>
#include <arch/io.h>
#include <soc/clock.h>

void clock_init(void)
{
	u32 freq = MVMAP2315_CLK_M_KHZ * 1000;

	set_cntfrq(freq);

	write32(&mvmap2315_gentimer->cntfid0, freq);

	setbits_le32(&mvmap2315_gentimer->cntcr, MVMAP2315_GENTIMER_EN);
}
