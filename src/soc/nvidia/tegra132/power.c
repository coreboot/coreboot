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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>
#include <console/console.h>
#include <soc/addressmap.h>

#include "pmc.h"
#include "power.h"

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
		write32(pwrgate_toggle, &pmc->pwrgate_toggle);

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
