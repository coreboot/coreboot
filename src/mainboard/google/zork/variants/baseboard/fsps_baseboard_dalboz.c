/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/bsd/compiler.h>

void __weak variant_get_pcie_ddi_descriptors(const fsp_pcie_descriptor **pcie_descs,
					     size_t *pcie_num,
					     const fsp_ddi_descriptor **ddi_descs,
					     size_t *ddi_num)
{
	*pcie_descs = baseboard_get_pcie_descriptors(pcie_num);
	*ddi_descs = baseboard_get_ddi_descriptors(ddi_num);
}

static const fsp_pcie_descriptor pcie_descriptors[] = {
	{
		// NVME SSD
		.port_present = true,
		.engine_type = PCIE_ENGINE,
		.start_lane = NVME_START_LANE,
		.end_lane = NVME_END_LANE,
		.device_number = 1,
		.function_number = 7,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = NVME_CLKREQ,
		.clk_pm_support = true,
	},
	{
		// WLAN
		.port_present = true,
		.engine_type = PCIE_ENGINE,
		.start_lane = WLAN_START_LANE,
		.end_lane = WLAN_END_LANE,
		.device_number = 1,
		.function_number = 2,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = WLAN_CLKREQ,
		.clk_pm_support = true,
	},
	{
		// SD Reader
		.port_present = true,
		.engine_type = PCIE_ENGINE,
		.start_lane = SD_START_LANE,
		.end_lane = SD_END_LANE,
		.device_number = 1,
		.function_number = 3,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = SD_CLKREQ,
	}
};

const fsp_pcie_descriptor *baseboard_get_pcie_descriptors(size_t *num)
{
	*num = ARRAY_SIZE(pcie_descriptors);
	return pcie_descriptors;
}

const fsp_ddi_descriptor *baseboard_get_ddi_descriptors(size_t *num)
{
	/* Different configurations of dalboz have different ddi configurations.
	 * Therefore, don't provide any baseboard defaults. */
	*num = 0;
	return NULL;
}
