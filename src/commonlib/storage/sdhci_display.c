/*
 * Copyright 2011, Marvell Semiconductor Inc.
 * Lei Wen <leiwen@marvell.com>
 *
 * Copyright 2017 Intel Corporation
 *
 * Secure Digital (SD) Host Controller interface specific code
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <commonlib/sd_mmc_ctrlr.h>
#include <commonlib/sdhci.h>
#include <commonlib/storage.h>
#include "sdhci.h"
#include "sd_mmc.h"
#include "storage.h"

static void sdhci_display_bus_width(struct sdhci_ctrlr *sdhci_ctrlr)
{
	if (IS_ENABLED(CONFIG_SDHC_DEBUG)) {
		int bits;
		uint8_t host_ctrl;
		uint16_t host2;
		const char *rate;
		uint16_t timing;

		/* Display the bus width */
		host_ctrl = sdhci_readb(sdhci_ctrlr, SDHCI_HOST_CONTROL);
		host2 = sdhci_readw(sdhci_ctrlr, SDHCI_HOST_CONTROL2);
		timing = host2 & SDHCI_CTRL_UHS_MASK;
		bits = 1;
		if (host_ctrl & SDHCI_CTRL_8BITBUS)
			bits = 8;
		else if (host_ctrl & SDHCI_CTRL_4BITBUS)
			bits = 4;
		rate = "SDR";
		if ((timing == SDHCI_CTRL_UHS_DDR50)
			|| (timing == SDHCI_CTRL_HS400))
			rate = "DDR";
		sdhc_debug("SDHCI bus width: %d bit%s %s\n", bits,
			(bits != 1) ? "s" : "", rate);
	}
}

static void sdhci_display_clock(struct sdhci_ctrlr *sdhci_ctrlr)
{
	if (IS_ENABLED(CONFIG_SDHC_DEBUG)) {
		uint16_t clk_ctrl;
		uint32_t clock;
		uint32_t divisor;

		/* Display the clock */
		clk_ctrl = sdhci_readw(sdhci_ctrlr, SDHCI_CLOCK_CONTROL);
		sdhc_debug("SDHCI bus clock: ");
		if (clk_ctrl & SDHCI_CLOCK_CARD_EN) {
			divisor = (clk_ctrl >> SDHCI_DIVIDER_SHIFT)
				& SDHCI_DIV_MASK;
			divisor |= ((clk_ctrl >> SDHCI_DIVIDER_SHIFT)
				<< SDHCI_DIV_MASK_LEN) & SDHCI_DIV_HI_MASK;
			divisor <<= 1;
			clock = sdhci_ctrlr->sd_mmc_ctrlr.clock_base;
			if (divisor)
				clock /= divisor;
			sdhc_debug("%d.%03d MHz\n", clock / 1000000,
				(clock / 1000) % 1000);
		} else
			sdhc_debug("Off\n");
	}
}

static void sdhci_display_voltage(struct sdhci_ctrlr *sdhci_ctrlr)
{
	if (IS_ENABLED(CONFIG_SDHC_DEBUG)) {
		u8 pwr_ctrl;
		const char *voltage;
		const char *voltage_table[8] = {
			"Unknown",	/* 0 */
			"Unknown",	/* 1 */
			"Unknown",	/* 2 */
			"Unknown",	/* 3 */
			"Unknown",	/* 4 */
			"1.8",		/* 5 */
			"3.0",		/* 6 */
			"3.3",		/* 7 */
		};

		pwr_ctrl = sdhci_readb(sdhci_ctrlr, SDHCI_POWER_CONTROL);
		if (pwr_ctrl & SDHCI_POWER_ON) {
			voltage = voltage_table[(pwr_ctrl & SDHCI_POWER_330)
				>> 1];
			sdhc_debug("SDHCI voltage: %s Volts\n", voltage);
		} else
			sdhc_debug("SDHCI voltage: Off\n");
	}
}

void sdhci_display_setup(struct sdhci_ctrlr *sdhci_ctrlr)
{
	/* Display the controller setup */
	sdhci_display_voltage(sdhci_ctrlr);
	sdhci_display_clock(sdhci_ctrlr);
	sdhci_display_bus_width(sdhci_ctrlr);
}
