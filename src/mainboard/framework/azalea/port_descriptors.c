/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/platform_descriptors.h>
static const fsp_dxio_descriptor dxio_descriptors[] = {
	{
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_lane = 16,
		.end_lane = 19,
		.device_number = 2,
		.function_number = 1,
		.link_speed_capability = GEN3,
		.turn_off_unused_lanes = true,
		.link_aspm = ASPM_DISABLED,	// TODO: Test ASPM
		.link_hotplug = HOTPLUG_DISABLED,
		.clk_req = CLK_REQ4,
	},
	{
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_lane = 15,
		.end_lane = 15,
		.device_number = 2,
		.function_number = 2,
		.link_speed_capability = GEN3,
		.turn_off_unused_lanes = true,
		.link_aspm = ASPM_DISABLED, // TODO: Test ASPM
		.link_hotplug = HOTPLUG_DISABLED,
		.clk_req = CLK_REQ6,
	},
};

static const fsp_ddi_descriptor ddi_descriptors[] = {
	{ /* DP0 - eDP */
		.connector_type = DDI_EDP,
		.aux_index = DDI_AUX1,
		.hdp_index = DDI_HDP1
	},
	{ /* DP1 - NC */
		.connector_type = DDI_UNUSED_TYPE,
		.aux_index = DDI_AUX2,
		.hdp_index = DDI_HDP2
	},
	{ /* DP2 - USBC1 */
		.connector_type = DDI_DP_W_TYPEC,
		.aux_index = DDI_AUX3,
		.hdp_index = DDI_HDP3,
	},
	{ /* DP3 - USBC4 */
		.connector_type = DDI_DP_W_TYPEC,
		.aux_index = DDI_AUX4,
		.hdp_index = DDI_HDP4,
	},
	{ /* DP4 - USBC2 */
		.connector_type = DDI_DP_W_TYPEC,
		.aux_index = DDI_AUX5,
		.hdp_index = DDI_HDP5,
	}
};

void mainboard_get_dxio_ddi_descriptors(
		const fsp_dxio_descriptor **dxio_descs, size_t *dxio_num,
		const fsp_ddi_descriptor **ddi_descs, size_t *ddi_num)
{
	*dxio_descs = dxio_descriptors;
	*dxio_num = ARRAY_SIZE(dxio_descriptors);
	*ddi_descs = ddi_descriptors;
	*ddi_num = ARRAY_SIZE(ddi_descriptors);
}
