/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
 * Copyright (c) 2013, NVIDIA CORPORATION.  All rights reserved.
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
#include <assert.h>
#include <console/console.h>
#include <soc/addressmap.h>
#include <soc/pmc.h>
#include <soc/power.h>

static struct tegra_pmc_regs * const pmc = (void *)TEGRA_PMC_BASE;

static int partition_powered(int id)
{
	return read32(&pmc->pwrgate_status) & (0x1 << id);
}

void power_ungate_partition(uint32_t id)
{
	printk(BIOS_INFO, "Ungating power partition %d.\n", id);

	if (!partition_powered(id)) {
		uint32_t pwrgate_toggle = read32(&pmc->pwrgate_toggle);
		pwrgate_toggle &= ~(PMC_PWRGATE_TOGGLE_PARTID_MASK);
		pwrgate_toggle |= (id << PMC_PWRGATE_TOGGLE_PARTID_SHIFT);
		pwrgate_toggle |= PMC_PWRGATE_TOGGLE_START;
		write32(&pmc->pwrgate_toggle, pwrgate_toggle);

		/* Wait for the request to be accepted. */
		while (read32(&pmc->pwrgate_toggle) & PMC_PWRGATE_TOGGLE_START)
			;
		printk(BIOS_DEBUG, "Power gate toggle request accepted.\n");

		/* Wait for the partition to be powered. */
		while (!partition_powered(id))
			;
	}

	printk(BIOS_INFO, "Ungated power partition %d.\n", id);
}

uint8_t pmc_rst_status(void)
{
	return read32(&pmc->rst_status) & PMC_RST_STATUS_SOURCE_MASK;
}

static const char *pmc_rst_status_str[PMC_RST_STATUS_NUM_SOURCES] = {
	[PMC_RST_STATUS_SOURCE_POR] = "POR",
	[PMC_RST_STATUS_SOURCE_WATCHDOG] = "Watchdog",
	[PMC_RST_STATUS_SOURCE_SENSOR] = "Sensor",
	[PMC_RST_STATUS_SOURCE_SW_MAIN] = "SW Main",
	[PMC_RST_STATUS_SOURCE_LP0] = "LP0",
};

void pmc_print_rst_status(void)
{
	uint8_t rst_status = pmc_rst_status();
	assert(rst_status < PMC_RST_STATUS_NUM_SOURCES);
	printk(BIOS_INFO, "PMC Reset Status: %s\n",
	       pmc_rst_status_str[rst_status]);
}
