/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/bsd/compiler.h>
#include <soc/soc_util.h>

void __weak variant_get_pcie_ddi_descriptors(const fsp_pcie_descriptor **pcie_descs,
					     size_t *pcie_num,
					     const fsp_ddi_descriptor **ddi_descs,
					     size_t *ddi_num)
{
	*pcie_descs = baseboard_get_pcie_descriptors(pcie_num);
	*ddi_descs = baseboard_get_ddi_descriptors(ddi_num);
}

/* FP5 package can support Type 1 (Picasso) and Type 2 (Dali), however some
 * Type 1 parts, while reporting as Picasso through cpuid, are fused like a Dali.
 * Those parts need to be configured as Type 2. */

static const fsp_pcie_descriptor pco_pcie_descriptors[] = {
	{
		// NVME SSD
		.port_present = true,
		.engine_type = PCIE_ENGINE,
		.start_lane = 0,
		.end_lane = 3,
		.device_number = 1,
		.function_number = 7,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = NVME_CLKREQ,
	},
	{
		// WLAN
		.port_present = true,
		.engine_type = PCIE_ENGINE,
		.start_lane = 4,
		.end_lane = 4,
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
		.start_lane = 5,
		.end_lane = 5,
		.device_number = 1,
		.function_number = 3,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = SD_CLKREQ,
	}
};

static const fsp_pcie_descriptor dali_pcie_descriptors[] = {
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
	/* Type 2 or Type 1 fused like Type 2. */
	if (soc_is_reduced_io_sku()) {
		*num = ARRAY_SIZE(dali_pcie_descriptors);
		return dali_pcie_descriptors;
	} else {
		/* Type 1 */
		*num = ARRAY_SIZE(pco_pcie_descriptors);
		return pco_pcie_descriptors;
	}

}

static const fsp_ddi_descriptor pco_ddi_descriptors[] = {
	{
		// DDI0, DP0, eDP
		.connector_type = EDP,
		.aux_index = AUX1,
		.hdp_index = HDP1
	},
	{
		// DDI1, DP1, DB OPT1 HDMI
		.connector_type = HDMI,
		.aux_index = AUX2,
		.hdp_index = HDP2
	},
	{
		// DDI2, DP2, DB OPT1 USB-C1
		.connector_type = DP,
		.aux_index = AUX3,
		.hdp_index = HDP3,
	},
	{
		// DDI3, DP3, USB-C0
		.connector_type = DP,
		.aux_index = AUX4,
		.hdp_index = HDP4,
	}
};

static const fsp_ddi_descriptor dali_ddi_descriptors[] = {
	{
		// DDI0, DP0, eDP
		.connector_type = EDP,
		.aux_index = AUX1,
		.hdp_index = HDP1
	},
	{
		// DDI1, DP1, DB OPT2 USB-C1 / DB OPT3 MST hub
		.connector_type = DP,
		.aux_index = AUX2,
		.hdp_index = HDP2
	},
	{
		// DDI2, DP3, USB-C0
		.connector_type = DP,
		.aux_index = AUX4,
		.hdp_index = HDP4,
	}
};

const fsp_ddi_descriptor *baseboard_get_ddi_descriptors(size_t *num)
{
	/* Type 2 or Type 1 fused like Type 2. */
	if (soc_is_reduced_io_sku()) {
		*num = ARRAY_SIZE(dali_ddi_descriptors);
		return dali_ddi_descriptors;
	} else {
		/* Type 1 */
		*num = ARRAY_SIZE(pco_ddi_descriptors);
		return pco_ddi_descriptors;
	}
}
