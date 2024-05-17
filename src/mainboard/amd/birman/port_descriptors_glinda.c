/* SPDX-License-Identifier: GPL-2.0-only */

#include <gpio.h>
#include <soc/platform_descriptors.h>
#include <types.h>
#include "display_card_type.h"

/* TODO: Update for birman */

static const fsp_dxio_descriptor birman_dxio_descriptors[] = {
	{
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 0,
		.end_logical_lane = 0,
		.device_number = 2,
		.function_number = 1,
		.link_speed_capability = GEN3,
		.turn_off_unused_lanes = true,
		.link_aspm = 2,
		.link_hotplug = HOTPLUG_ENHANCED,
		.clk_req = CLK_REQ3,
	},
	{
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 1,
		.end_logical_lane = 1,
		.device_number = 2,
		.function_number = 2,
		.link_speed_capability = GEN3,
		.turn_off_unused_lanes = true,
		.link_aspm = 2,
		.link_hotplug = HOTPLUG_ENHANCED,
		.clk_req = CLK_REQ1,
	},
	{
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 2,
		.end_logical_lane = 3,
		.device_number = 2,
		.function_number = 3,
		.link_speed_capability = GEN3,
		.turn_off_unused_lanes = true,
		.link_aspm = 2,
		.link_hotplug = HOTPLUG_ENHANCED,
		.gpio_group_id = GPIO_27,
		.clk_req = CLK_REQ0,
	},
};

static fsp_ddi_descriptor birman_ddi_descriptors[] = {
	{ /* DDI0 - eDP */
		.connector_type = DDI_EDP,
		.aux_index = DDI_AUX1,
		.hdp_index = DDI_HDP1
	},
	{ /* DDI1 - HDMI/DP */
		.connector_type = DDI_HDMI,
		.aux_index = DDI_AUX2,
		.hdp_index = DDI_HDP2
	},
	{ /* DDI2 - DP (type C) */
		.connector_type = DDI_DP_W_TYPEC,
		.aux_index = DDI_AUX3,
		.hdp_index = DDI_HDP3,
	},
	{ /* DDI3 - DP (type C) */
		.connector_type = DDI_DP_W_TYPEC,
		.aux_index = DDI_AUX4,
		.hdp_index = DDI_HDP4,
	},
	{ /* DDI4 - DP (type C) */
		.connector_type = DDI_DP_W_TYPEC,
		.aux_index = DDI_AUX5,
		.hdp_index = DDI_HDP5,
	}
};

void mainboard_get_dxio_ddi_descriptors(
		const fsp_dxio_descriptor **dxio_descs, size_t *dxio_num,
		const fsp_ddi_descriptor **ddi_descs, size_t *ddi_num)
{
	birman_ddi_descriptors[1].connector_type = get_ddi1_type();

	*dxio_descs = birman_dxio_descriptors;
	*dxio_num = ARRAY_SIZE(birman_dxio_descriptors);
	*ddi_descs = birman_ddi_descriptors;
	*ddi_num = ARRAY_SIZE(birman_ddi_descriptors);
}
