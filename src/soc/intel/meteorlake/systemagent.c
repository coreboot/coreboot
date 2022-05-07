/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <delay.h>
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
	struct device *sa;
	uint16_t sa_pci_id;
	config_t *config;

	/* Enable Power Aware Interrupt Routing */
	enable_power_aware_intr();

	/* Enable BIOS Reset CPL */
	enable_bios_reset_cpl();

	/* Configure turbo power limits 1ms after reset complete bit */
	mdelay(1);
	config = config_of_soc();

	/* Get System Agent PCI ID */
	sa = pcidev_path_on_root(PCI_DEVFN_ROOT);
	sa_pci_id = sa ? pci_read_config16(sa, PCI_DEVICE_ID) : 0xFFFF;

	/* Choose a power limits configuration based on the SoC SKU type,
	 * differentiated here based on SA PCI ID. */
	switch (sa_pci_id) {
	case PCI_DID_INTEL_MTL_P_ID_1:
		soc_config = &config->power_limits_config[MTL_P_POWER_LIMITS_1];
		break;
	case PCI_DID_INTEL_MTL_P_ID_2:
		soc_config = &config->power_limits_config[MTL_P_POWER_LIMITS_2];
		break;
	case PCI_DID_INTEL_MTL_P_ID_3:
		soc_config = &config->power_limits_config[MTL_P_POWER_LIMITS_3];
		break;
	default:
		printk(BIOS_ERR, "unknown SA ID: 0x%4x, skipping power limits configuration\n",
			sa_pci_id);
		return;
	}

	/* UPDATEME: Need to enable later */
	//set_power_limits(MOBILE_SKU_PL1_TIME_SEC, soc_config);
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
		return 65536;
	}
}
