/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <gpio.h>
#include <soc/platform_descriptors.h>
#include <soc/soc_util.h>
#include <types.h>
#include "display_card_type.h"

#define phx_mxm_dxio_descriptor {			\
	.engine_type = PCIE_ENGINE,			\
	.port_present = CONFIG(ENABLE_EVAL_CARD),	\
	.start_lane = 0,				\
	.end_lane = 7,					\
	.device_number = 1,				\
	.function_number = 1,				\
	.link_speed_capability = GEN3,			\
	.turn_off_unused_lanes = true,			\
	.link_aspm = ASPM_L1,				\
	.link_hotplug = HOTPLUG_DISABLED,		\
	.clk_req = CLK_REQ0,				\
}

/* TODO: verify on hardware */
#define phx2_mxm_dxio_descriptor {			\
	.engine_type = PCIE_ENGINE,			\
	.port_present = CONFIG(ENABLE_EVAL_CARD),	\
	.start_lane = 0,				\
	.end_lane = 3,					\
	.device_number = 1,				\
	.function_number = 1,				\
	.link_speed_capability = GEN3,			\
	.turn_off_unused_lanes = true,			\
	.link_aspm = ASPM_L1,				\
	.link_hotplug = HOTPLUG_DISABLED,		\
	.clk_req = CLK_REQ0,				\
}

#define phx_ssd1_dxio_descriptor {			\
	.engine_type = PCIE_ENGINE,			\
	.port_present = !CONFIG(DISABLE_DT_M2),		\
	.start_lane = 8,				\
	.end_lane = 11,					\
	.device_number = 1,				\
	.function_number = 2,				\
	.link_speed_capability = GEN3,			\
	.turn_off_unused_lanes = true,			\
	.link_aspm = ASPM_L1,				\
	.link_hotplug = HOTPLUG_DISABLED,		\
	.clk_req = CLK_REQ1,				\
}

/* TODO: verify on hardware */
#define phx2_ssd1_dxio_descriptor {			\
	.engine_type = PCIE_ENGINE,			\
	.port_present = true,				\
	.start_lane = 8,				\
	.end_lane = 9,					\
	.device_number = 1,				\
	.function_number = 2,				\
	.link_speed_capability = GEN3,			\
	.turn_off_unused_lanes = true,			\
	.link_aspm = ASPM_L1,				\
	.link_hotplug = HOTPLUG_DISABLED,		\
	.clk_req = CLK_REQ1,				\
}

#define gbe_dxio_descriptor {				\
	.engine_type = PCIE_ENGINE,			\
	.port_present = true,				\
	.start_lane = 12,				\
	.end_lane = 12,					\
	.device_number = 1,				\
	.function_number = 3,				\
	.link_speed_capability = GEN3,			\
	.turn_off_unused_lanes = true,			\
	.link_aspm = ASPM_DISABLED,			\
	.link_hotplug = HOTPLUG_DISABLED,		\
	.clk_req = CLK_REQ6,				\
}

#define sd_dxio_descriptor {				\
	.engine_type = PCIE_ENGINE,			\
	.port_present = true,				\
	.start_lane = 13,				\
	.end_lane = 13,					\
	.device_number = 2,				\
	.function_number = 1,				\
	.link_speed_capability = GEN3,			\
	.turn_off_unused_lanes = true,			\
	.link_aspm = ASPM_DISABLED,			\
	.link_hotplug = HOTPLUG_DISABLED,		\
	.clk_req = CLK_REQ5,				\
}

#define wwan_dxio_descriptor {				\
	.engine_type = PCIE_ENGINE,			\
	.port_present = true,				\
	.start_lane = 14,				\
	.end_lane = CONFIG(WWAN01) ? 15 : 14,		\
	.device_number = 2,				\
	.function_number = 2,				\
	.link_speed_capability = GEN3,			\
	.turn_off_unused_lanes = true,			\
	.link_aspm = ASPM_DISABLED,			\
	.link_hotplug = HOTPLUG_DISABLED,		\
	.clk_req = CLK_REQ4,				\
}

#define wlan_dxio_descriptor {				\
	.engine_type = PCIE_ENGINE,			\
	.port_present = true,				\
	.start_lane = 15,				\
	.end_lane = CONFIG(WLAN01) ? 14 : 15,		\
	.device_number = 2,				\
	.function_number = 3,				\
	.link_speed_capability = GEN3,			\
	.turn_off_unused_lanes = true,			\
	.link_aspm = ASPM_DISABLED,			\
	.link_hotplug = HOTPLUG_DISABLED,		\
	.clk_req = CLK_REQ3,				\
}

#define ssd0_dxio_descriptor {				\
	.engine_type = PCIE_ENGINE,			\
	.port_present = true,				\
	.start_lane = 16,				\
	.end_lane = 19,					\
	.device_number = 2,				\
	.function_number = 4,				\
	.link_speed_capability = GEN3,			\
	.turn_off_unused_lanes = true,			\
	.link_aspm = ASPM_DISABLED,			\
	.link_hotplug = HOTPLUG_DISABLED,		\
	.clk_req = CLK_REQ2,				\
}

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

	enum soc_type type = get_soc_type();

	if (type == SOC_PHOENIX) {
		printk(BIOS_DEBUG, "Using PHX DXIO\n");
		static const fsp_dxio_descriptor birman_phx_dxio_descriptors[] = {
			phx_mxm_dxio_descriptor,
			phx_ssd1_dxio_descriptor,
			gbe_dxio_descriptor,
			sd_dxio_descriptor,
#if CONFIG(WLAN0_WWAN0) || CONFIG(WWAN01)
			wwan_dxio_descriptor,
#endif
#if CONFIG(WLAN0_WWAN0) || CONFIG(WLAN01)
			wlan_dxio_descriptor,
#endif
			ssd0_dxio_descriptor,
		};
		*dxio_descs = birman_phx_dxio_descriptors;
		*dxio_num = ARRAY_SIZE(birman_phx_dxio_descriptors);
	} else {
		printk(BIOS_DEBUG, "Using PHX2 DXIO\n");
		static const fsp_dxio_descriptor birman_phx2_dxio_descriptors[] = {
			phx2_mxm_dxio_descriptor,
			phx2_ssd1_dxio_descriptor,
			gbe_dxio_descriptor,
			sd_dxio_descriptor,
#if CONFIG(WLAN0_WWAN0) || CONFIG(WWAN01)
			wwan_dxio_descriptor,
#endif
#if CONFIG(WLAN0_WWAN0) || CONFIG(WLAN01)
			wlan_dxio_descriptor,
#endif
			ssd0_dxio_descriptor,
		};
		*dxio_descs = birman_phx2_dxio_descriptors;
		*dxio_num = ARRAY_SIZE(birman_phx2_dxio_descriptors);
	}

	*ddi_descs = birman_ddi_descriptors;
	*ddi_num = ARRAY_SIZE(birman_ddi_descriptors);
}
