/* SPDX-License-Identifier: GPL-2.0-only */

#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <intelblocks/power_limit.h>
#include <intelblocks/systemagent.h>
#include <soc/iomap.h>
#include <soc/soc_chip.h>
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
		{ PCIEXBAR, CONFIG_ECAM_MMCONF_BASE_ADDRESS, CONFIG_ECAM_MMCONF_LENGTH,
				"PCIEXBAR" },
		{ MCHBAR, MCH_BASE_ADDRESS, MCH_BASE_SIZE, "MCHBAR" },
		{ DMIBAR, DMI_BASE_ADDRESS, DMI_BASE_SIZE, "DMIBAR" },
		{ EPBAR, EP_BASE_ADDRESS, EP_BASE_SIZE, "EPBAR" },
		{ REGBAR, REG_BASE_ADDRESS, REG_BASE_SIZE, "REGBAR" },
		{ EDRAMBAR, EDRAM_BASE_ADDRESS, EDRAM_BASE_SIZE, "EDRAMBAR" },
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
	struct soc_power_limits_config *soc_config;
	config_t *config;
	uint32_t value;

	/* Enable Power Aware Interrupt Routing */
	enable_power_aware_intr();

	/* Enable BIOS Reset CPL */
	enable_bios_reset_cpl();

	mdelay(1);
	if (CONFIG(SOC_INTEL_DISABLE_POWER_LIMITS)) {
		printk(BIOS_INFO, "Skip setting RAPL per configuration\n");
		/* clear bits 47, 15 in PACKAGE_RAPL_LIMIT_0_0_0_MCHBAR_PCU */
		value = MCHBAR32(MCH_PKG_POWER_LIMIT_LO);
		MCHBAR32(MCH_PKG_POWER_LIMIT_LO) = value & ~(PKG_PWR_LIM_1_EN);
		value = MCHBAR32(MCH_PKG_POWER_LIMIT_HI);
		MCHBAR32(MCH_PKG_POWER_LIMIT_HI) = value & ~(PKG_PWR_LIM_2_EN);
	} else {
		config = config_of_soc();
		soc_config = &config->power_limits_config;
		set_power_limits(MOBILE_SKU_PL1_TIME_SEC, soc_config);
	}
}
