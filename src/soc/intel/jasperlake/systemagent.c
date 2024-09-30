/* SPDX-License-Identifier: GPL-2.0-only */

#include <chip.h>
#include <console/console.h>
#include <device/device.h>
#include <delay.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <intelblocks/systemagent.h>
#include <intelblocks/power_limit.h>
#include <soc/iomap.h>
#include <soc/soc_chip.h>
#include <soc/systemagent.h>
#include <static.h>

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
	uint16_t sa_pci_id;
	uint8_t tdp;
	size_t i = 0;

	/* Enable Power Aware Interrupt Routing */
	enable_power_aware_intr();

	/* Enable BIOS Reset CPL */
	enable_bios_reset_cpl();

	mdelay(1);
	config = config_of_soc();

	/* Get System Agent PCI ID */
	sa_pci_id = dev ? pci_read_config16(dev, PCI_DEVICE_ID) : 0xFFFF;

	if (sa_pci_id != 0xFFFF) {
		tdp = get_cpu_tdp();

		/* Choose power limits configuration based on the CPU SA PCI ID and
		* CPU TDP value. */
		for (i = 0; i < ARRAY_SIZE(cpuid_to_jsl); i++) {
			if (sa_pci_id == cpuid_to_jsl[i].pci_did &&
					tdp == cpuid_to_jsl[i].cpu_tdp) {
				soc_config = &config->power_limits_config[cpuid_to_jsl[i].limits];
				set_power_limits(MOBILE_SKU_PL1_TIME_SEC, soc_config);
				break;
			}
		}
	}

	if (i == ARRAY_SIZE(cpuid_to_jsl) || sa_pci_id == 0xFFFF) {
		printk(BIOS_ERR, "unknown SA ID: 0x%4x, can't find its TDP."
			" Skipped power limits configuration.\n",
			sa_pci_id);
		return;
	}
}
