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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/io.h>
#include <console/console.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/flow.h>
#include <soc/pmc.h>
#include <soc/power.h>

static struct tegra_pmc_regs * const pmc = (void *)TEGRA_PMC_BASE;
static struct flow_ctlr * const flow = (void *)TEGRA_FLOW_BASE;

static int partition_powered(int id)
{
	return read32(&pmc->pwrgate_status) & (0x1 << id);
}

static int partition_clamp_on(int id)
{
	return read32(&pmc->clamp_status) & (0x1 << id);
}

static void power_ungate_partition(uint32_t id)
{
	printk(BIOS_INFO, "Ungating power partition %d.\n", id);

	if (!partition_powered(id)) {
		uint32_t pwrgate_toggle = read32(&pmc->pwrgate_toggle);
		pwrgate_toggle &= ~(PMC_PWRGATE_TOGGLE_PARTID_MASK);
		pwrgate_toggle |= (id << PMC_PWRGATE_TOGGLE_PARTID_SHIFT);
		pwrgate_toggle |= PMC_PWRGATE_TOGGLE_START;
		write32(&pmc->pwrgate_toggle, pwrgate_toggle);

		// Wait for the request to be accepted.
		while (read32(&pmc->pwrgate_toggle) & PMC_PWRGATE_TOGGLE_START)
			;
		printk(BIOS_DEBUG, "Power gate toggle request accepted.\n");

		// Wait for the partition to be powered.
		while (!partition_powered(id))
			;

		// Wait for clamp off.
		while (partition_clamp_on(id))
			;
	}

	printk(BIOS_INFO, "Ungated power partition %d.\n", id);
}

void power_enable_and_ungate_cpu(void)
{
	/*
	 * Set CPUPWRGOOD_TIMER - APB clock is 1/2 of SCLK (150MHz),
	 * set it for 5ms as per SysEng (5ms * PCLK_KHZ * 1000 / 1s).
	 */
	write32(&pmc->cpupwrgood_timer, (TEGRA_PCLK_KHZ * 5));

	uint32_t cntrl = read32(&pmc->cntrl);
	cntrl &= ~PMC_CNTRL_CPUPWRREQ_POLARITY;
	cntrl |= PMC_CNTRL_CPUPWRREQ_OE;
	write32(&pmc->cntrl, cntrl);

	power_ungate_partition(POWER_PARTID_CRAIL);

	// Ungate power to the non-core parts of the fast cluster.
	power_ungate_partition(POWER_PARTID_C0NC);

	// Ungate power to CPU0 in the fast cluster.
	power_ungate_partition(POWER_PARTID_CE0);
}

int power_reset_status(void)
{
	return read32(&pmc->rst_status) & 0x7;
}

void ram_repair(void)
{
	// Request RAM repair for cluster 0
	setbits_le32(&flow->ram_repair, RAM_REPAIR_REQ);
	// Poll for completion
	while (!(read32(&flow->ram_repair) & RAM_REPAIR_STS))
		;
	// Request RAM repair for cluster 1
	setbits_le32(&flow->ram_repair_cluster1, RAM_REPAIR_REQ);
	// Poll for completion
	while (!(read32(&flow->ram_repair_cluster1) & RAM_REPAIR_STS))
		;
}
