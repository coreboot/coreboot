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

#include <arch/io.h>
#include <console/console.h>
#include <delay.h>
#include <soc/addressmap.h>
#include <soc/emmc.h>

#define SDHCI_CLOCK_CONTROL	0x2c
#define SDHCI_CLOCK_CARD_EN	0x0004
#define SDHCI_CLOCK_INT_STABLE	0x0002
#define SDHCI_CLOCK_INT_EN	0x0001

/* TODO(crosbug.com/p/52873): We actually don't need to set clk for
 * emmc once kernel fix it's bug.
 */
void enable_emmc_clk(void)
{
	int timeout, clk;

	write32((void *)(EMMC_BASE + SDHCI_CLOCK_CONTROL), SDHCI_CLOCK_INT_EN);

	/* Wait max 20 ms */
	timeout = 20;
	while (!((clk = read32((void *)(EMMC_BASE + SDHCI_CLOCK_CONTROL)))
		& SDHCI_CLOCK_INT_STABLE)) {
		if (timeout == 0) {
			printk(BIOS_ERR, "Internal clock never stabilised.\n");
			return;
		}
		timeout--;
		udelay(1000);
	}

	clk |= SDHCI_CLOCK_CARD_EN;
	write32((void *)(EMMC_BASE + SDHCI_CLOCK_CONTROL), clk);
}
