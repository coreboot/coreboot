/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017-2018 Intel Corporation.
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
#include <intelblocks/chip.h>
#include <intelblocks/lpc_lib.h>
#include <intelpch/lockdown.h>
#include <soc/pm.h>

static void lpc_lockdown_config(int chipset_lockdown)
{
	/* Set Bios Interface Lock, Bios Lock */
	if (chipset_lockdown == CHIPSET_LOCKDOWN_COREBOOT) {
		lpc_set_bios_interface_lock_down();
		lpc_set_lock_enable();
	}
}

static void pmc_lockdown_config(void)
{
	uint8_t *pmcbase;
	u32 pmsyncreg;

	/* PMSYNC */
	pmcbase = pmc_mmio_regs();
	pmsyncreg = read32(pmcbase + PMSYNC_TPR_CFG);
	pmsyncreg |= PMSYNC_LOCK;
	write32(pmcbase + PMSYNC_TPR_CFG, pmsyncreg);
}

void soc_lockdown_config(int chipset_lockdown)
{
	/* LPC lock down configuration */
	lpc_lockdown_config(chipset_lockdown);

	/* PMC lock down configuration */
	pmc_lockdown_config();
}
