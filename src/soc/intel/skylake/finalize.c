/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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
#include <console/console.h>
#include <console/post_codes.h>
#include <cpu/x86/smm.h>
#include <reg_script.h>
#include <spi-generic.h>
#include <stdlib.h>
#include <soc/lpc.h>
#include <soc/pci_devs.h>
#include <soc/pcr.h>
#include <soc/pm.h>
#include <soc/pmc.h>
#include <soc/spi.h>
#include <soc/systemagent.h>
#include <device/pci.h>

static void pch_finalize_script(void)
{
	device_t dev;
	uint32_t reg32, hsfs;
	void *spibar = get_spi_bar();
	u16 tcobase;
	u16 tcocnt;
	uint8_t *pmcbase;
	u32 pmsyncreg;

	/* Set SPI opcode menu */
	write16(spibar + SPIBAR_PREOP, SPI_OPPREFIX);
	write16(spibar + SPIBAR_OPTYPE, SPI_OPTYPE);
	write32(spibar + SPIBAR_OPMENU_LOWER, SPI_OPMENU_LOWER);
	write32(spibar + SPIBAR_OPMENU_UPPER, SPI_OPMENU_UPPER);
	/* Lock SPIBAR */
	hsfs = read32(spibar + SPIBAR_HSFS);
	hsfs |= SPIBAR_HSFS_FLOCKDN;
	write32(spibar + SPIBAR_HSFS, hsfs);

	/*TCO Lock down */
	tcobase = pmc_tco_regs();
	tcocnt = inw(tcobase + TCO1_CNT);
	tcocnt |= TCO_LOCK;
	outw(tcocnt, tcobase + TCO1_CNT);

	/* Lock down ABASE and sleep stretching policy */
	dev = PCH_DEV_PMC;
	reg32 = pci_read_config32(dev, GEN_PMCON_B);
	reg32 |= (SLP_STR_POL_LOCK | ACPI_BASE_LOCK);
	pci_write_config32(dev, GEN_PMCON_B, reg32);

	/* PMSYNC */
	pmcbase = pmc_mmio_regs();
	pmsyncreg = read32(pmcbase + PMSYNC_TPR_CFG);
	pmsyncreg |= PMSYNC_LOCK;
	write32(pmcbase + PMSYNC_TPR_CFG, pmsyncreg);
}

static void soc_lockdown(void)
{
	u8 reg8;
	device_t dev;
	const struct device *dev1 = dev_find_slot(0, PCH_DEVFN_LPC);
	const struct soc_intel_skylake_config *config = dev1->chip_info;

	/* Global SMI Lock */
	if (config->LockDownConfigGlobalSmi == 0) {
		dev = PCH_DEV_PMC;
		reg8 = pci_read_config8(dev, GEN_PMCON_A);
		reg8 |= SMI_LOCK;
		pci_write_config8(dev, GEN_PMCON_A, reg8);
	}

	/* Bios Interface Lock */
	if (config->LockDownConfigBiosInterface == 0) {
		pci_write_config8(PCH_DEV_LPC, BIOS_CNTL,
				  pci_read_config8(PCH_DEV_LPC,
						   BIOS_CNTL) | LPC_BC_BILD);
		/* Reads back for posted write to take effect */
		pci_read_config8(PCH_DEV_LPC, BIOS_CNTL);
		pci_write_config32(PCH_DEV_SPI, SPIBAR_BIOS_CNTL,
				   pci_read_config32(PCH_DEV_SPI,
						     SPIBAR_BIOS_CNTL) |
				   SPIBAR_BC_BILD);
		/* Reads back for posted write to take effect */
		pci_read_config32(PCH_DEV_SPI, SPIBAR_BIOS_CNTL);
		/* GCS reg of DMI */
		pcr_andthenor8(PID_DMI, R_PCH_PCR_DMI_GCS, 0xFF,
			       B_PCH_PCR_DMI_GCS_BILD);
	}

	/* Bios Lock */
	if (config->LockDownConfigBiosLock == 0) {
		pci_write_config8(PCH_DEV_LPC, BIOS_CNTL,
				  pci_read_config8(PCH_DEV_LPC,
						   BIOS_CNTL) | LPC_BC_LE);
		pci_write_config8(PCH_DEV_SPI, BIOS_CNTL,
				  pci_read_config8(PCH_DEV_SPI,
						   BIOS_CNTL) | SPIBAR_BC_LE);
	}

	/* SPIEiss */
	if (config->LockDownConfigSpiEiss == 0) {
		pci_write_config8(PCH_DEV_LPC, BIOS_CNTL,
				  pci_read_config8(PCH_DEV_LPC,
						   BIOS_CNTL) | LPC_BC_EISS);
		pci_write_config8(PCH_DEV_SPI, BIOS_CNTL,
				  pci_read_config8(PCH_DEV_SPI,
						   SPIBAR_BIOS_CNTL) |
				  SPIBAR_BC_EISS);
	}
}

static void soc_finalize(void *unused)
{
	printk(BIOS_DEBUG, "Finalizing chipset.\n");

	pch_finalize_script();

	soc_lockdown();

	/* Indicate finalize step with post code */
	post_code(POST_OS_BOOT);
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, soc_finalize, NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT, soc_finalize, NULL);
