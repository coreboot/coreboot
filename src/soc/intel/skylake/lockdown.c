/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corporation.
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
#include <bootstate.h>
#include <chip.h>
#include <soc/lpc.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <string.h>

static void lpc_lockdown_config(void)
{
	static struct soc_intel_skylake_config *config;
	struct device *dev;
	uint8_t reg_mask = 0;

	dev = PCH_DEV_LPC;
	/* Check if LPC is enabled, else return */
	if (dev == NULL || dev->chip_info == NULL)
		return;

	config = dev->chip_info;

	/* Set Bios Interface Lock, Bios Lock */
	if (config->chipset_lockdown == CHIPSET_LOCKDOWN_COREBOOT)
		reg_mask |= LPC_BC_BILD | LPC_BC_LE;

	pci_or_config8(dev, BIOS_CNTL, reg_mask);
	/* Ensure an additional read back after performing lock down */
	pci_read_config8(dev, BIOS_CNTL);
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

static void platform_lockdown_config(void *unused)
{
	/* LPC lock down configuration */
	lpc_lockdown_config();

	/* PMC lock down configuration */
	pmc_lockdown_config();
}

BOOT_STATE_INIT_ENTRY(BS_DEV_RESOURCES, BS_ON_EXIT, platform_lockdown_config,
				NULL);
