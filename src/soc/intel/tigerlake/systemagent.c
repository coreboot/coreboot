/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on Intel Tiger Lake Processor SA Datasheet
 * Document number: 571131
 * Chapter number: 3
 */

#include <console/console.h>
#include <device/device.h>
#include <delay.h>
#include <device/pci.h>
#include <device/pci_ids.h>
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
	struct device *sa;
	uint16_t sa_pci_id;
	config_t *config;

	/* Get System Agent PCI ID */
	sa = pcidev_path_on_root(SA_DEVFN_ROOT);
	sa_pci_id = sa ? pci_read_config16(sa, PCI_DEVICE_ID) : 0xFFFF;

	/* Enable Power Aware Interrupt Routing */
	enable_power_aware_intr();

	/* Enable BIOS Reset CPL */
	enable_bios_reset_cpl();

	/* Configure turbo power limits 1ms after reset complete bit */
	mdelay(1);
	config = config_of_soc();

	/*
	 * Choose a power limits configuration based on the SoC SKU,
	 * differentiated here based on SA PCI ID.
	 */
	switch (sa_pci_id) {
	case PCI_DID_INTEL_TGL_ID_U_2_2:
		soc_config = &config->power_limits_config[POWER_LIMITS_U_2_CORE];
		break;
	case PCI_DID_INTEL_TGL_ID_U_4_2:
		soc_config = &config->power_limits_config[POWER_LIMITS_U_4_CORE];
		break;
	case PCI_DID_INTEL_TGL_ID_Y_2_2:
		soc_config = &config->power_limits_config[POWER_LIMITS_Y_2_CORE];
		break;
	case PCI_DID_INTEL_TGL_ID_Y_4_2:
		soc_config = &config->power_limits_config[POWER_LIMITS_Y_4_CORE];
		break;
	case PCI_DID_INTEL_TGL_ID_H_6_1:
		soc_config = &config->power_limits_config[POWER_LIMITS_H_6_CORE];
		break;
	case PCI_DID_INTEL_TGL_ID_H_8_1:
		soc_config = &config->power_limits_config[POWER_LIMITS_H_8_CORE];
		break;
	default:
		printk(BIOS_ERR, "TGL: unknown SA ID: 0x%4x, skipping power limits "
		       "configuration\n", sa_pci_id);
		return;
	}

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
		return 65536;
	}
}
