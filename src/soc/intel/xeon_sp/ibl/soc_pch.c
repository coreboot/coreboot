/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <device/pci_ops.h>
#include <soc/bootblock.h>
#include <soc/pci_devs.h>
#include <soc/pmc.h>
#include <soc/soc_pch.h>

static void soc_config_acpibase(void)
{
	/* Disable IO command in PMC Device first before changing Base Address */
	uint16_t reg16 = pci_read_config16(PCH_DEV_PMC, PCI_COMMAND);
	pci_write_config16(PCH_DEV_PMC, PCI_COMMAND,
			reg16 & ~(PCI_COMMAND_IO | PCI_COMMAND_MASTER));

	/* Program ACPI Base */
	pci_write_config32(PCH_DEV_PMC, ABASE, ACPI_BASE_ADDRESS);

	/* Enable Bus Master and IO Space */
	pci_or_config16(PCH_DEV_PMC, PCI_COMMAND, (PCI_COMMAND_IO | PCI_COMMAND_MASTER));

	uint32_t data = pci_read_config32(PCH_DEV_PMC, ABASE);
	assert(ACPI_BASE_ADDRESS == (data & ~PCI_BASE_ADDRESS_IO_ATTR_MASK));
	printk(BIOS_INFO, "%s : pmbase = %x\n", __func__, (int)data);
}

void bootblock_pch_init(void)
{
	/*
	 * Enabling ABASE for accessing PM1_STS, PM1_EN, PM1_CNT
	 */
	soc_config_acpibase();
}

void early_pch_init(void)
{
}
