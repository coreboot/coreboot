/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ops.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <intelblocks/pcr.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/rtc.h>
#include <intelblocks/p2sb.h>
#include <soc/azalia_device.h>
#include <soc/bootblock.h>
#include <soc/soc_pch.h>
#include <soc/pch_pci_devs.h>
#include <soc/pch.h>
#include <soc/pmc.h>
#include <console/console.h>

#define PCR_PSF3_TO_SHDW_PMC_REG_BASE	0x600
#define PCR_PSFX_TO_SHDW_BAR4		0x10
#define PCR_PSFX_TO_SHDW_PCIEN_IOEN	0x01
#define PCR_PSFX_T0_SHDW_PCIEN		0x1C
#define PCR_DMI_MISC_PORT_CFG		0x20D8
#define  MISC_PORT_CFG_LOCK		BIT(23)
#define PCR_DMI_DMICTL		0x2234
#define  PCR_DMI_DMICTL_SRLOCK	(1 << 31)

static void soc_config_acpibase(void)
{
	uint32_t pmc_reg_value;
	uint32_t pmc_base_reg = PCR_PSF3_TO_SHDW_PMC_REG_BASE;

	pmc_reg_value = pcr_read32(PID_PSF3, pmc_base_reg + PCR_PSFX_TO_SHDW_BAR4);

	if (pmc_reg_value != 0xffffffff) {
		/* Disable Io Space before changing the address */
		pcr_rmw32(PID_PSF3, pmc_base_reg + PCR_PSFX_T0_SHDW_PCIEN,
				~PCR_PSFX_TO_SHDW_PCIEN_IOEN, 0);
		/* Program ABASE in PSF3 PMC space BAR4*/
		pcr_write32(PID_PSF3, pmc_base_reg + PCR_PSFX_TO_SHDW_BAR4,
				ACPI_BASE_ADDRESS);
		/* Enable IO Space */
		pcr_rmw32(PID_PSF3, pmc_base_reg + PCR_PSFX_T0_SHDW_PCIEN,
				~0, PCR_PSFX_TO_SHDW_PCIEN_IOEN);
	}
	/* Enable Bus Master and IO Space */
	pci_or_config16(PCH_DEV_PMC, PCI_COMMAND, (PCI_COMMAND_IO | PCI_COMMAND_MASTER));

	uint16_t data = pcr_read16(PID_PSF3, pmc_base_reg + PCR_PSFX_TO_SHDW_BAR4);
	printk(BIOS_INFO, "%s : pmbase = %x\n", __func__, (int)data);
}

void bootblock_pch_init(void)
{
	/*
	 * Enabling ABASE for accessing PM1_STS, PM1_EN, PM1_CNT
	 */
	soc_config_acpibase();
}

void pch_lock_dmictl(void)
{
	uint32_t reg32 = pcr_read32(PID_DMI, PCR_DMI_DMICTL);
	pcr_write32(PID_DMI, PCR_DMI_DMICTL, reg32 | PCR_DMI_DMICTL_SRLOCK);
	pcr_or32(PID_DMI, PCR_DMI_MISC_PORT_CFG, MISC_PORT_CFG_LOCK);
}

#define PCR_PSFX_T0_SHDW_PCIEN		0x1C
#define PCR_PSFX_T0_SHDW_PCIEN_FUNDIS	(1 << 8)
#define PSF3_HDA_BASE_ADDRESS		0x280

static void pch_disable_hda(void)
{
	/* Ensure memory, io, and bus master are all disabled */
	pci_and_config16(PCH_DEV_HDA, PCI_COMMAND, ~(PCI_COMMAND_MASTER |
			 PCI_COMMAND_MEMORY | PCI_COMMAND_IO));
	/* Put controller to D3 */
	pci_or_config32(PCH_DEV_HDA, HDA_PCS, HDA_PCS_PS_D3HOT);
	/* Disable DSP in PMC */
	pmc_or_mmio32(NST_PG_FDIS1, NST_FDIS_DSP);
	/* Hide PCI function */
	pcr_or32(PID_PSF3, PSF3_HDA_BASE_ADDRESS + PCR_PSFX_T0_SHDW_PCIEN,
		 PCR_PSFX_T0_SHDW_PCIEN_FUNDIS);

	printk(BIOS_INFO, "%s: Disabled HDA device 00:1f.3\n", __func__);
}

void early_pch_init(void)
{
	/* FSP has no UPD to disable HDA, so do it manually here... */
	if (!is_devfn_enabled(PCH_DEVFN_HDA))
		pch_disable_hda();
}
