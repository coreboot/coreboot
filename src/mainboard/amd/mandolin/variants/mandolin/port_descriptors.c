/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/platform_descriptors.h>
#include <soc/soc_util.h>
#include <types.h>

static const fsp_pcie_descriptor pco_pcie_descriptors[] = {
	{ /* MXM */
		.port_present = true,
		.engine_type = PCIE_ENGINE,
		.start_lane = 8,
		.end_lane = 15,
		.device_number = 1,
		.function_number = 1,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ6
	},
	{ /* SSD */
		.port_present = true,
		.engine_type = PCIE_ENGINE,
		.start_lane = 0,
		.end_lane = 1,
		.device_number = 1,
		.function_number = 7,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ5
	},
	{ /* WLAN */
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
		.clk_req = CLK_REQ0
	},
	{ /* LAN */
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
		.clk_req = CLK_REQ1
	},
	{ /* WWAN */
		.port_present = true,
		.engine_type = PCIE_ENGINE,
		.start_lane = 6,
		.end_lane = 6,
		.device_number = 1,
		.function_number = 4,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ2
	},
	{ /* WIFI */
		.port_present = true,
		.engine_type = PCIE_ENGINE,
		.start_lane = 7,
		.end_lane = 7,
		.gpio_group_id = 1,
		.device_number = 1,
		.function_number = 5,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ3
	},
	{ /* SATA EXPRESS */
		.port_present = true,
		.engine_type = SATA_ENGINE,
		.start_lane = 2,
		.end_lane = 3,
		.gpio_group_id = 1,
		.channel_type = SATA_CHANNEL_LONG,
	}
};

static const fsp_pcie_descriptor dali_pcie_descriptors[] = {
	{ /* MXM */
		.port_present = true,
		.engine_type = PCIE_ENGINE,
		.start_lane = 8,
		.end_lane = 11,
		.device_number = 1,
		.function_number = 1,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ6
	},
	{ /* SSD */
		.port_present = true,
		.engine_type = PCIE_ENGINE,
		.start_lane = 0,
		.end_lane = 1,
		.device_number = 1,
		.function_number = 7,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ5
	},
	{ /* WLAN */
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
		.clk_req = CLK_REQ0
	},
	{ /* LAN */
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
		.clk_req = CLK_REQ1
	},
	{ /* SATA */
		.port_present = true,
		.engine_type = SATA_ENGINE,
		.start_lane = 2,
		.end_lane = 3,
		.gpio_group_id = 1,
		.channel_type = SATA_CHANNEL_LONG,
	}
};

static const fsp_ddi_descriptor pco_ddi_descriptors[] = {
	{ /* DDI0 - DP */
		.connector_type = DP,
		.aux_index = AUX1,
		.hdp_index = HDP1
	},
	{ /* DDI1 - eDP */
		.connector_type = EDP,
		.aux_index = AUX2,
		.hdp_index = HDP2
	},
	{ /* DDI2 - DP */
		.connector_type = DP,
		.aux_index = AUX3,
		.hdp_index = HDP3,
	},
	{ /* DDI3 - DP */
		.connector_type = DP,
		.aux_index = AUX4,
		.hdp_index = HDP4,
	}
};

static const fsp_ddi_descriptor dali_ddi_descriptors[] = {
	{ /* DDI0 - DP */
		.connector_type = DP,
		.aux_index = AUX1,
		.hdp_index = HDP1
	},
	{ /* DDI1 - eDP */
		.connector_type = EDP,
		.aux_index = AUX2,
		.hdp_index = HDP2
	},
	{ /* DDI2 - DP */
		.connector_type = DP,
		.aux_index = AUX3,
		.hdp_index = HDP3,
	}
};

void mainboard_get_pcie_ddi_descriptors(
		const fsp_pcie_descriptor **pcie_descs, size_t *pcie_num,
		const fsp_ddi_descriptor **ddi_descs, size_t *ddi_num)
{
	if (soc_is_reduced_io_sku()) { /* Dali */
		*pcie_descs = dali_pcie_descriptors;
		*pcie_num = ARRAY_SIZE(dali_pcie_descriptors);
		*ddi_descs = dali_ddi_descriptors;
		*ddi_num = ARRAY_SIZE(dali_ddi_descriptors);
	} else { /* Picasso and default */
		*pcie_descs = pco_pcie_descriptors;
		*pcie_num = ARRAY_SIZE(pco_pcie_descriptors);
		*ddi_descs = pco_ddi_descriptors;
		*ddi_num = ARRAY_SIZE(pco_ddi_descriptors);
	}
}
