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
#include <device/pci.h>
#include <intelblocks/fast_spi.h>
#include <intelblocks/pcr.h>
#include <reg_script.h>
#include <spi-generic.h>
#include <soc/lpc.h>
#include <soc/me.h>
#include <soc/p2sb.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <soc/pm.h>
#include <soc/smbus.h>
#include <soc/systemagent.h>
#include <stdlib.h>

#define PCR_DMI_GCS		0x274C
#define PCR_DMI_GCS_BILD  	(1 << 0)
#define PSF_BASE_ADDRESS	0xA00
#define PCR_PSFX_T0_SHDW_PCIEN	0x1C
#define PCR_PSFX_T0_SHDW_PCIEN_FUNDIS	(1 << 8)

static void pch_configure_endpoints(device_t dev, int epmask_id, uint32_t mask)
{
	uint32_t reg32;

	reg32 = pci_read_config32(dev, PCH_P2SB_EPMASK(epmask_id));
	pci_write_config32(dev, PCH_P2SB_EPMASK(epmask_id), reg32 | mask);
}

static void pch_disable_heci(void)
{
	device_t dev;
	u8 reg8;
	uint32_t mask;

	dev = PCH_DEV_P2SB;

	/*
	 * if p2sb device 1f.1 is not present or hidden in devicetree
	 * p2sb device becomes NULL
	 */
	if (!dev)
		return;

	/* unhide p2sb device */
	pci_write_config8(dev, PCH_P2SB_E0 + 1, 0);

	/* disable heci */
	pcr_or32(PID_PSF1, PSF_BASE_ADDRESS + PCR_PSFX_T0_SHDW_PCIEN,
		PCR_PSFX_T0_SHDW_PCIEN_FUNDIS);

	/* Remove the host accessing right to PSF register range. */
	/* Set p2sb PCI offset EPMASK5 C4h [29, 28, 27, 26] to [1, 1, 1, 1] */
	mask = (1 << 29) | (1 << 28) | (1 << 27)  | (1 << 26);
	pch_configure_endpoints(dev, 5, mask);

	/* Set the "Endpoint Mask Lock!", P2SB PCI offset E2h bit[1] to 1. */
	reg8 = pci_read_config8(dev, PCH_P2SB_E0 + 2);
	pci_write_config8(dev, PCH_P2SB_E0 + 2, reg8 | (1 << 1));

	/* hide p2sb device */
	pci_write_config8(dev, PCH_P2SB_E0 + 1, 1);
}

static void pch_finalize_script(void)
{
	device_t dev;
	uint32_t reg32;
	u16 tcobase;
	u16 tcocnt;
	uint8_t *pmcbase;
	config_t *config;
	u32 pmsyncreg;
	u8 reg8;

	/* Set FAST_SPI opcode menu */
	fast_spi_set_opcode_menu();

	/* Lock FAST_SPIBAR */
	fast_spi_lock_bar();

	/*TCO Lock down */
	tcobase = smbus_tco_regs();
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

	/* Display me status before we hide it */
	intel_me_status();

	/* we should disable Heci1 based on the devicetree policy */
	config = dev->chip_info;

	/*
	 * Disable ACPI PM timer based on dt policy
	 *
	 * Disabling ACPI PM timer is necessary for XTAL OSC shutdown.
	 * Disabling ACPI PM timer also switches off TCO
	 */

	if (config->PmTimerDisabled) {
		reg8 = read8(pmcbase + PCH_PWRM_ACPI_TMR_CTL);
		reg8 |= (1 << 1);
		write8(pmcbase + PCH_PWRM_ACPI_TMR_CTL, reg8);
	}

	/* Disable XTAL shutdown qualification for low power idle. */
	if (config->s0ix_enable) {
		reg32 = read32(pmcbase + CIR31C);
		reg32 |= XTALSDQDIS;
		write32(pmcbase + CIR31C, reg32);
	}

	/* we should disable Heci1 based on the devicetree policy */
	if (config->HeciEnabled == 0)
		pch_disable_heci();
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

		fast_spi_set_bios_interface_lock_down();

		/* GCS reg of DMI */
		pcr_or8(PID_DMI, PCR_DMI_GCS, PCR_DMI_GCS_BILD);
	}

	/* Bios Lock */
	if (config->LockDownConfigBiosLock == 0) {
		pci_write_config8(PCH_DEV_LPC, BIOS_CNTL,
				  pci_read_config8(PCH_DEV_LPC,
						   BIOS_CNTL) | LPC_BC_LE);

		/* Ensure an additional read back after performing lock down */
		pci_read_config8(PCH_DEV_LPC, BIOS_CNTL);

		fast_spi_set_lock_enable();
	}

	/* SPIEiss */
	if (config->LockDownConfigSpiEiss == 0) {
		pci_write_config8(PCH_DEV_LPC, BIOS_CNTL,
				  pci_read_config8(PCH_DEV_LPC,
						   BIOS_CNTL) | LPC_BC_EISS);

		/* Ensure an additional read back after performing lock down */
		pci_read_config8(PCH_DEV_LPC, BIOS_CNTL);

		fast_spi_set_eiss();
	}
}

static void soc_finalize(void *unused)
{
	printk(BIOS_DEBUG, "Finalizing chipset.\n");

	pch_finalize_script();

	soc_lockdown();

	printk(BIOS_DEBUG, "Finalizing SMM.\n");
	outb(APM_CNT_FINALIZE, APM_CNT);

	/* Indicate finalize step with post code */
	post_code(POST_OS_BOOT);
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, soc_finalize, NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT, soc_finalize, NULL);
