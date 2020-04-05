/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <intelblocks/systemagent.h>
#include <soc/iomap.h>
#include <soc/systemagent.h>

/*
 * SoC implementation
 *
 * Add all known fixed memory ranges for Host Controller/Memory
 * controller.
 */
void soc_add_fixed_mmio_resources(struct device *dev, int *index)
{
	static const struct sa_mmio_descriptor soc_fixed_resources[] = {
		{ PCIEXBAR, CONFIG_MMCONF_BASE_ADDRESS, CONFIG_SA_PCIEX_LENGTH,
				"PCIEXBAR" },
		{ MCHBAR, MCH_BASE_ADDRESS, MCH_BASE_SIZE, "MCHBAR" },
		{ DMIBAR, DMI_BASE_ADDRESS, DMI_BASE_SIZE, "DMIBAR" },
		{ EPBAR, EP_BASE_ADDRESS, EP_BASE_SIZE, "EPBAR" },
		{ REGBAR, REG_BASE_ADDRESS, REG_BASE_SIZE, "REGBAR" },
		{ EDRAMBAR, EDRAM_BASE_ADDRESS, EDRAM_BASE_SIZE, "EDRAMBAR" },
		/*
		 * PMC pci device gets hidden from PCI bus due to Silicon
		 * policy hence binding PMCBAR aka PWRMBASE (offset 0x10) with
		 * SA resources to ensure that PMCBAR falls under PCI reserved
		 * memory range.
		 *
		 * Note: Don't add any more resource with same offset 0x10
		 * under this device space.
		 */
		{ PCI_BASE_ADDRESS_0, PCH_PWRM_BASE_ADDRESS, PCH_PWRM_BASE_SIZE,
				"PMCBAR" },
	};

	sa_add_fixed_mmio_resources(dev, index, soc_fixed_resources,
			ARRAY_SIZE(soc_fixed_resources));

	/* Add Vt-d resources if VT-d is enabled */
	if ((pci_read_config32(dev, CAPID0_A) & VTD_DISABLE))
		return;

	sa_add_fixed_mmio_resources(dev, index, soc_vtd_resources,
			ARRAY_SIZE(soc_vtd_resources));
}

/*
 * SoC implementation
 *
 * Perform System Agent Initialization during Ramstage phase.
 */
void soc_systemagent_init(struct device *dev)
{
	/* Enable Power Aware Interrupt Routing */
	enable_power_aware_intr();

	/* Enable BIOS Reset CPL */
	enable_bios_reset_cpl();
}
