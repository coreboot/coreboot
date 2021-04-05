/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/platform_descriptors.h>
#include <soc/gpio.h>
#include <types.h>

/* TODO: test if this really works */
static const fsp_dxio_descriptor guybrush_czn_dxio_descriptors[] = {
	{ /* WLAN */
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 0,
		.end_logical_lane = 0,
		.device_number = 2,
		.function_number = 1,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ0,
		.gpio_group_id = GPIO_29,
		.port_params = {PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122}
	},
	{ /* SD */
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 1,
		.end_logical_lane = 1,
		.device_number = 2,
		.function_number = 2,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ1,
		.gpio_group_id = GPIO_70,
		.port_params = {PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122}
	},
	{ /* WWAN */
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 2,
		.end_logical_lane = 2,
		.device_number = 2,
		.function_number = 3,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ2,
		.gpio_group_id = GPIO_18,
		.port_params = {PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122}
	},
	{ /* NVME */
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 4,
		.end_logical_lane = 7,
		.device_number = 2,
		.function_number = 4,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ3,
		.gpio_group_id = GPIO_40,
		.port_params = {PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122}
	}
};

/* TODO: verify the DDI table, since this is mostly an educated guess right now */
static const fsp_ddi_descriptor guybrush_czn_ddi_descriptors[] = {
	{ /* DDI0 - eDP */
		.connector_type = DDI_EDP,
		.aux_index = DDI_AUX1,
		.hdp_index = DDI_HDP1
	},
	{ /* DDI1 - HDMI */
		.connector_type = DDI_HDMI,
		.aux_index = DDI_AUX2,
		.hdp_index = DDI_HDP2
	},
	{ /* DDI2 */
		.connector_type = DDI_UNUSED_TYPE,
		.aux_index = DDI_AUX3,
		.hdp_index = DDI_HDP3,
	},
	{ /* DDI3 - DP (type C) */
		.connector_type = DDI_DP,
		.aux_index = DDI_AUX3,
		.hdp_index = DDI_HDP3,
	},
	{ /* DDI4 - DP (type C) */
		.connector_type = DDI_DP,
		.aux_index = DDI_AUX4,
		.hdp_index = DDI_HDP4,
	}
};

void mainboard_get_dxio_ddi_descriptors(
		const fsp_dxio_descriptor **dxio_descs, size_t *dxio_num,
		const fsp_ddi_descriptor **ddi_descs, size_t *ddi_num)
{
	*dxio_descs = guybrush_czn_dxio_descriptors;
	*dxio_num = ARRAY_SIZE(guybrush_czn_dxio_descriptors);
	*ddi_descs = guybrush_czn_ddi_descriptors;
	*ddi_num = ARRAY_SIZE(guybrush_czn_ddi_descriptors);
}
