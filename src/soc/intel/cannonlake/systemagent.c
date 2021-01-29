/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <delay.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <intelblocks/power_limit.h>
#include <intelblocks/systemagent.h>
#include <soc/cpu.h>
#include <soc/iomap.h>
#include <soc/systemagent.h>
#include "chip.h"

/*
 * SoC implementation
 *
 * Add all known fixed memory ranges for Host Controller/Memory
 * controller.
 */
void soc_add_fixed_mmio_resources(struct device *dev, int *index)
{
	static const struct sa_mmio_descriptor soc_fixed_resources[] = {
		{ PCIEXBAR, CONFIG_MMCONF_BASE_ADDRESS, CONFIG_MMCONF_LENGTH,
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

	/* Add Vt-d resources if VT-d is enabled. */
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
	struct soc_power_limits_config *soc_config;
	config_t *config;

	/* Enable Power Aware Interrupt Routing */
	enable_power_aware_intr();

	/* Enable BIOS Reset CPL */
	enable_bios_reset_cpl();

	/* Configure turbo power limits 1ms after reset complete bit */
	mdelay(1);
	config = config_of_soc();
	soc_config = &config->power_limits_config;
	set_power_limits(MOBILE_SKU_PL1_TIME_SEC, soc_config);
}

uint32_t soc_systemagent_max_chan_capacity_mib(u8 capid0_a_ddrsz)
{
	switch (capid0_a_ddrsz) {
	case 1:
		return 8192;
	case 2:
		return 4096;
	case 3:
		return 2048;
	default:
		return 32768;
	}
}
