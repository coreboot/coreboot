/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/platform_descriptors.h>
#include <types.h>
#include <gpio.h>

/*
 * Rave2 board PCIe / DDI configuration
 *
 * DDI:   4x mini DisplayPort 2.1 (CN9-CN12) from APU
 * DXIO:  M.2 M-key PCIe 4.0 x4 NVMe (M2)          - lanes  0-3
 *        10GbE AQC113 controller (lower RJ45)       - lane  12
 *        M.2 E-key PCIe 4.0 x1 WiFi/BT (M1)        - lane  13
 *        VE3558 PCIe x2                             - lanes 14-15
 *        OcuLink PCIe 4.0 x4 (J6)                  - lanes 16-19
 *
 * DisplayPort configuration:
 * DDI0-2: Mini DisplayPort 2.1 (CN9-CN11) from APU
 * DDI3: USB Type C from APU
 * DDI4: Mini DisplayPort 2.1 (CN12) from APU
 */
static const fsp_ddi_descriptor rave2_ddi_descriptors[] = {
	{ /* DDI0 - CN9 mini DP */
		.connector_type = DDI_DP,
		.aux_index = DDI_AUX1,
		.hdp_index = DDI_HDP1
	},
	{ /* DDI1 - CN10 mini DP */
		.connector_type = DDI_DP,
		.aux_index = DDI_AUX2,
		.hdp_index = DDI_HDP2
	},
	{ /* DDI2 - CN11 mini DP */
		.connector_type = DDI_DP,
		.aux_index = DDI_AUX3,
		.hdp_index = DDI_HDP3,
	},
	{ /* DDI3 - USB Type C */
		.connector_type = DDI_DP_W_TYPEC,
		.aux_index = DDI_AUX4,
		.hdp_index = DDI_HDP4,
	},
	{ /* DDI4 - CN12 mini DP */
		.connector_type = DDI_DP,
		.aux_index = DDI_AUX5,
		.hdp_index = DDI_HDP5,
	},
};

static const fsp_dxio_descriptor rave2_dxio_descriptors[] = {
	{ /* M.2 M-key NVMe PCIe 4.0 x4 - M2 (M2M lanes 0-3) */
		.engine_type = PCIE_ENGINE,
		.gpio_group_id = GPIO_8,
		.port_present = true,
		.start_logical_lane = 0,
		.end_logical_lane = 3,
		.device_number = 2,
		.function_number = 1,
		.link_speed_capability = GEN_MAX,
		.turn_off_unused_lanes = true,
		.link_aspm = ASPM_L1,
		.link_hotplug = HOTPLUG_DISABLED,
		.clk_req = CLK_REQ0,
		.eq_preset = 3,
		.port_params = {PP_PSPP_AC, 0x144, PP_PSPP_DC, 0x133},
	},
	{ /* 10GbE AQC113 controller - lower RJ45 (LAN_CLK GPP_CLK2, CLK_REQ2, lane 12) */
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 12,
		.end_logical_lane = 12,
		.device_number = 3,
		.function_number = 1,
		.link_speed_capability = GEN_MAX,
		.turn_off_unused_lanes = true,
		.link_aspm = ASPM_L1,
		.link_hotplug = HOTPLUG_DISABLED,
		.clk_req = CLK_REQ2,
		.port_params = {PP_PSPP_AC, 0x144, PP_PSPP_DC, 0x133},
	},
	{ /* M.2 E-key WiFi/BT PCIe 4.0 x1 - M1 (M2E_CLK GPP_CLK3, CLK_REQ3, lane 13) */
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 13,
		.end_logical_lane = 13,
		.device_number = 3,
		.function_number = 2,
		.link_speed_capability = GEN_MAX,
		.turn_off_unused_lanes = true,
		.link_aspm = ASPM_L1,
		.link_hotplug = HOTPLUG_DISABLED,
		.clk_req = CLK_REQ3,
	},
	{ /* VE3558 PCIe x2 (VE3558_CLK GPP_CLK4, no CLKREQ# pin, lanes 14-15) */
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 14,
		.end_logical_lane = 15,
		.device_number = 3,
		.function_number = 3,
		.link_speed_capability = GEN_MAX,
		.turn_off_unused_lanes = true,
		.link_aspm = ASPM_L1,
		.link_hotplug = HOTPLUG_BASIC,
		.clk_req = CLK_DISABLE,
		.port_params = {PP_PSPP_AC, 0x144, PP_PSPP_DC, 0x133},
	},
	{ /* OcuLink PCIe 4.0 x4 - J6 (GFX_CLK GPP_CLK5, no CLKREQ# pin, lanes 16-19) */
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 16,
		.end_logical_lane = 19,
		.device_number = 3,
		.function_number = 4,
		.link_speed_capability = GEN_MAX,
		.turn_off_unused_lanes = true,
		.link_aspm = ASPM_L1,
		.link_hotplug = HOTPLUG_BASIC,
		.clk_req = CLK_DISABLE,
		.port_params = {PP_PSPP_AC, 0x144, PP_PSPP_DC, 0x133},
	},
};

void mainboard_get_dxio_ddi_descriptors(
		const fsp_dxio_descriptor **dxio_descs, size_t *dxio_num,
		const fsp_ddi_descriptor **ddi_descs, size_t *ddi_num)
{
	*dxio_descs = rave2_dxio_descriptors;
	*dxio_num = ARRAY_SIZE(rave2_dxio_descriptors);
	*ddi_descs = rave2_ddi_descriptors;
	*ddi_num = ARRAY_SIZE(rave2_ddi_descriptors);
}
