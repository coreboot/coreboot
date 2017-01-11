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
#include <soc/clock.h>
#include <soc/reset.h>

void __attribute__((noreturn)) mvmap2315_reset(void)
{
	clrbits_le32(&mvmap2315_mpmu_clk->resetmcu, MVMAP2315_MCU_RST_EN);
	clrbits_le32(&mvmap2315_mpmu_clk->resetap, MVMAP2315_AP_RST_EN);
	for (;;) ;
}
