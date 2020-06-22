/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/platform_descriptors.h>
#include <types.h>

static const fsp_dxio_descriptor pollock_dxio_descriptors[] = {
	{ /* NVME SSD */
		.port_present = true,
		.engine_type = PCIE_ENGINE,
		.start_logical_lane = 0,
		.end_logical_lane = 0,
		.device_number = 1,
		.function_number = 3,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ0
	},
	{ /* WWAN */
		.port_present = true,
		.engine_type = PCIE_ENGINE,
		.start_logical_lane = 1,
		.end_logical_lane = 1,
		.device_number = 1,
		.function_number = 4,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ2
	},
	{ /* LAN */
		.port_present = true,
		.engine_type = PCIE_ENGINE,
		.start_logical_lane = 4,
		.end_logical_lane = 4,
		.device_number = 1,
		.function_number = 1,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ1
	},
	{ /* WLAN */
		.port_present = true,
		.engine_type = PCIE_ENGINE,
		.start_logical_lane = 5,
		.end_logical_lane = 5,
		.device_number = 1,
		.function_number = 2,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ4
	}
};

fsp_ddi_descriptor pollock_ddi_descriptors[] = {
	{ /* DDI0 - eDP */
		.connector_type = EDP,
		.aux_index = AUX1,
		.hdp_index = HDP1
	},
	{ /* DDI1 - DP */
		.connector_type = DP,
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
	*dxio_descs = pollock_dxio_descriptors;
	*dxio_num = ARRAY_SIZE(pollock_dxio_descriptors);
	*ddi_descs = pollock_ddi_descriptors;
	*ddi_num = ARRAY_SIZE(pollock_ddi_descriptors);
}
