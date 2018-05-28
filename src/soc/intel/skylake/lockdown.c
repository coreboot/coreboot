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
#include <intelblocks/fast_spi.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/pcr.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <soc/pm.h>
#include <string.h>

#define PCR_DMI_GCS		0x274C
#define PCR_DMI_GCS_BILD	(1 << 0)

static void lpc_lockdown_config(const struct soc_intel_skylake_config *config)
{
	/* Set Bios Interface Lock, Bios Lock */
	if (config->chipset_lockdown == CHIPSET_LOCKDOWN_COREBOOT) {
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

static void dmi_lockdown_config(void)
{
	/*
	 * GCS reg of DMI
	 *
	 * When set, prevents GCS.BBS from being changed
	 * GCS.BBS: (Boot BIOS Strap) This field determines the destination
	 * of accesses to the BIOS memory range.
	 *	Bits Description
	 *	"0b": SPI
	 *	"1b": LPC/eSPI
	 */
	pcr_or8(PID_DMI, PCR_DMI_GCS, PCR_DMI_GCS_BILD);
}

static void spi_lockdown_config(const struct soc_intel_skylake_config *config)
{
	/* Set FAST_SPI opcode menu */
	fast_spi_set_opcode_menu();

	/* Discrete Lock Flash PR registers */
	fast_spi_pr_dlock();

	/* Lock FAST_SPIBAR */
	fast_spi_lock_bar();

	/* Set Bios Interface Lock, Bios Lock */
	if (config->chipset_lockdown == CHIPSET_LOCKDOWN_COREBOOT) {
		/* Bios Interface Lock */
		fast_spi_set_bios_interface_lock_down();

		/* Bios Lock */
		fast_spi_set_lock_enable();
	}
}

static void platform_lockdown_config(void *unused)
{
	struct soc_intel_skylake_config *config;
	struct device *dev;

	dev = PCH_DEV_SPI;
	/* Check if device is valid, else return */
	if (dev == NULL || dev->chip_info == NULL)
		return;

	config = dev->chip_info;

	/* LPC lock down configuration */
	lpc_lockdown_config(config);

	/* SPI lock down configuration */
	spi_lockdown_config(config);

	/* DMI lock down configuration */
	dmi_lockdown_config();

	/* PMC lock down configuration */
	pmc_lockdown_config();
}

BOOT_STATE_INIT_ENTRY(BS_DEV_RESOURCES, BS_ON_EXIT, platform_lockdown_config,
				NULL);
