/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ops.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <intelblocks/pcr.h>
#include <intelblocks/rtc.h>
#include <intelblocks/p2sb.h>
#include <soc/bootblock.h>
#include <soc/soc_pch.h>
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
