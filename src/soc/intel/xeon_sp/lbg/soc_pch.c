/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ops.h>
#include <intelblocks/pcr.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/rtc.h>
#include <intelblocks/p2sb.h>
#include <soc/azalia_device.h>
#include <soc/bootblock.h>
#include <soc/soc_pch.h>
#include <soc/pch_pci_devs.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <soc/pmc.h>
#include <console/console.h>

#define PCR_DMI_ACPIBA		0x27B4
#define PCR_DMI_ACPIBDID	0x27B8
#define PCR_DMI_DMICTL		0x2234
#define  PCR_DMI_DMICTL_SRLOCK	(1 << 31)
#define PCR_DMI_PMBASEA		0x27AC
#define PCR_DMI_PMBASEC		0x27B0

static void soc_config_acpibase(void)
{
	uint32_t reg32;

	/* Disable ABASE in PMC Device first before changing Base Address */
	reg32 = pci_read_config32(PCH_DEV_PMC, ACTL);
	pci_write_config32(PCH_DEV_PMC, ACTL, reg32 & ~ACPI_EN);

	/* Program ACPI Base */
	pci_write_config32(PCH_DEV_PMC, ABASE, ACPI_BASE_ADDRESS);

	/* Enable ACPI in PMC */
	pci_write_config32(PCH_DEV_PMC, ACTL, reg32 | ACPI_EN);

	uint32_t data = pci_read_config32(PCH_DEV_PMC, ABASE);
	printk(BIOS_INFO, "%s : pmbase = %x\n", __func__, (int)data);
	/*
	 * Program "ACPI Base Address" PCR[DMI] + 27B4h[23:18, 15:2, 0]
	 * to [0x3F, PMC PCI Offset 40h bit[15:2], 1]
	 */
	reg32 = (0x3f << 18) | ACPI_BASE_ADDRESS | 1;
	pcr_write32(PID_DMI, PCR_DMI_ACPIBA, reg32);
	pcr_write32(PID_DMI, PCR_DMI_ACPIBDID, 0x23a8);
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
}

#define PCR_PSFX_T0_SHDW_PCIEN		0x1C
#define PCR_PSFX_T0_SHDW_PCIEN_FUNDIS	(1 << 8)
#define PSF3_HDA_BASE_ADDRESS		0x1800

static void pch_disable_hda(void)
{
	/* Ensure memory, io, and bus master are all disabled */
	pci_and_config16(PCH_DEV_HDA, PCI_COMMAND, ~(PCI_COMMAND_MASTER |
			 PCI_COMMAND_MEMORY | PCI_COMMAND_IO));
	/* Put controller to D3 */
	pci_or_config32(PCH_DEV_HDA, HDA_PCS, HDA_PCS_PS_D3HOT);
	/* Lock and disable everything */
	pci_write_config8(PCH_DEV_HDA, HDA_FNCFG, 0x15);

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
