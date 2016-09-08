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
#include <console/console.h>
#include <soc/wdt.h>

struct mvmap2315_wdt_regs *mvmap2315_wdts[] = {
	(struct mvmap2315_wdt_regs *)MVMAP2315_WDT0_BASE,
	(struct mvmap2315_wdt_regs *)MVMAP2315_WDT1_BASE,
};

void enable_sp_watchdog(u32 timer, u32 timeout_value)
{
	u32 reg;

	reg = read32(&mvmap2315_wdts[timer]->wdt_cr);

	/* set watchdog Reset pulse length to the highest value
	 * (256 Pclk cycles)
	 */
	reg |= 7 << MVMAP2315_WDT_CR_RPL_SHIFT;

	/* set the watchdog timer to generate a system reset */
	reg &= ~MVMAP2315_WDT_CR_RMOD;

	write32(&mvmap2315_wdts[timer]->wdt_cr, reg);

	/* enable the watchdog timer */
	setbits_le32(&mvmap2315_wdts[timer]->wdt_cr, MVMAP2315_WDT_CR_EN);

	/* set watchdog Timeout Range Register */
	reg = timeout_value << MVMAP2315_WDT_TORR_TOP_SHIFT;
	write32(&mvmap2315_wdts[timer]->wdt_torr, reg);

	/* reset the timer */
	reg = 0x76 << MVMAP2315_WDT_CRR_SHIFT;
	write32(&mvmap2315_wdts[timer]->wdt_crr, reg);
}

void reset_sp_watchdog(u32 timer, u32 timeout_value)
{
	u32 reg;

	/* set watchdog Timeout Range Register */
	reg = timeout_value << MVMAP2315_WDT_TORR_TOP_SHIFT;
	write32(&mvmap2315_wdts[timer]->wdt_torr, reg);

	if (read32(&mvmap2315_wdts[timer]->wdt_cr) & MVMAP2315_WDT_CR_EN) {
		/* reset the timer */
		reg = 0x76 << MVMAP2315_WDT_CRR_SHIFT;
		write32(&mvmap2315_wdts[timer]->wdt_crr, reg);
	} else {
		/* enable the watchdog timer */
		setbits_le32(&mvmap2315_wdts[timer]->wdt_cr,
			     MVMAP2315_WDT_CR_EN);
	}
}
