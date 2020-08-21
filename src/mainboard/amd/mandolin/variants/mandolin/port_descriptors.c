/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/platform_descriptors.h>
#include <soc/soc_util.h>
#include <types.h>

static const fsp_dxio_descriptor pco_dxio_descriptors[] = {
	{ /* MXM */
		.port_present = true,
		.engine_type = PCIE_ENGINE,
		.start_logical_lane = 8,
		.end_logical_lane = 15,
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
		.start_logical_lane = 0,
		.end_logical_lane = 1,
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
		.start_logical_lane = 4,
		.end_logical_lane = 4,
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
		.start_logical_lane = 5,
		.end_logical_lane = 5,
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
		.start_logical_lane = 6,
		.end_logical_lane = 6,
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
		.start_logical_lane = 7,
		.end_logical_lane = 7,
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
		.start_logical_lane = 2,
		.end_logical_lane = 3,
		.gpio_group_id = 1,
		.channel_type = SATA_CHANNEL_LONG,
	}
};

static const fsp_dxio_descriptor dali_dxio_descriptors[] = {
	{ /* MXM */
		.port_present = true,
		.engine_type = PCIE_ENGINE,
		.start_logical_lane = 8,
		.end_logical_lane = 11,
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
		.start_logical_lane = 0,
		.end_logical_lane = 1,
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
		.start_logical_lane = 4,
		.end_logical_lane = 4,
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
		.start_logical_lane = 5,
		.end_logical_lane = 5,
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
		.start_logical_lane = 2,
		.end_logical_lane = 3,
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
		.aux_index = AUX4,
		.hdp_index = HDP4,
	}
};

void mainboard_get_dxio_ddi_descriptors(
		const fsp_dxio_descriptor **dxio_descs, size_t *dxio_num,
		const fsp_ddi_descriptor **ddi_descs, size_t *ddi_num)
{
	if (soc_is_reduced_io_sku()) { /* Dali */
		*dxio_descs = dali_dxio_descriptors;
		*dxio_num = ARRAY_SIZE(dali_dxio_descriptors);
		*ddi_descs = dali_ddi_descriptors;
		*ddi_num = ARRAY_SIZE(dali_ddi_descriptors);
	} else { /* Picasso and default */
		*dxio_descs = pco_dxio_descriptors;
		*dxio_num = ARRAY_SIZE(pco_dxio_descriptors);
		*ddi_descs = pco_ddi_descriptors;
		*ddi_num = ARRAY_SIZE(pco_ddi_descriptors);
	}
}
