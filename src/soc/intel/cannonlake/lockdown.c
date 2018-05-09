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
#include <intelblocks/chip.h>
#include <intelblocks/fast_spi.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/pcr.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <soc/pm.h>
#include <string.h>

#define PCR_DMI_GCS		0x274C
#define PCR_DMI_GCS_BILD	(1 << 0)

static void pmc_lockdown_cfg(const struct soc_intel_common_config *config)
{
	uint8_t *pmcbase, reg8;
	uint32_t reg32, pmsyncreg;

	/* PMSYNC */
	pmcbase = pmc_mmio_regs();
	pmsyncreg = read32(pmcbase + PMSYNC_TPR_CFG);
	pmsyncreg |= PCH2CPU_TPR_CFG_LOCK;
	write32(pmcbase + PMSYNC_TPR_CFG, pmsyncreg);

	/* Lock down ABASE and sleep stretching policy */
	reg32 = read32(pmcbase + GEN_PMCON_B);
	reg32 |= (SLP_STR_POL_LOCK | ACPI_BASE_LOCK);
	write32(pmcbase + GEN_PMCON_B, reg32);

	if (config->chipset_lockdown == CHIPSET_LOCKDOWN_COREBOOT) {
		pmcbase = pmc_mmio_regs();
		reg8 = read8(pmcbase + GEN_PMCON_B);
		reg8 |= SMI_LOCK;
		write8(pmcbase + GEN_PMCON_B, reg8);
	}
}

static void dmi_lockdown_cfg(void)
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

static void fast_spi_lockdown_cfg(const struct soc_intel_common_config *config)
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
	const struct soc_intel_common_config *common_config;
	common_config = chip_get_common_soc_structure();

	/* SPI lock down configuration */
	fast_spi_lockdown_cfg(common_config);

	/* DMI lock down configuration */
	dmi_lockdown_cfg();

	/* PMC lock down configuration */
	pmc_lockdown_cfg(common_config);
}

BOOT_STATE_INIT_ENTRY(BS_DEV_RESOURCES, BS_ON_EXIT, platform_lockdown_config,
				NULL);
