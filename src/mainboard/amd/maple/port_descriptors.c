/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/i2c_simple.h>
#include <gpio.h>
#include <soc/platform_descriptors.h>
#include <types.h>

#define strix_halo_ssd1_dxio_descriptor {				\
	.engine_type = PCIE_ENGINE,			\
	.port_present = true,				\
	.start_logical_lane = 0,				\
	.end_logical_lane = 3,					\
	.device_number = 3,				\
	.function_number = 1,				\
	.link_speed_capability = GEN_MAX,			\
	.turn_off_unused_lanes = true,			\
	.link_aspm = ASPM_L1,				\
	.link_aspm_L1_1 = true, \
	.link_aspm_L1_2 = true, \
	.link_hotplug = HOTPLUG_DISABLED,		\
	.clk_req = CLK_REQ0, \
	.port_params = {PP_PSPP_AC, 0x144, PP_PSPP_DC, 0x133}, \
}

#define strix_halo_dt_mpcie_dxio_descriptor {				\
	.engine_type = PCIE_ENGINE,			\
	.port_present = true,				\
	.start_logical_lane = 4,				\
	.end_logical_lane = 7,					\
	.device_number = 3,				\
	.function_number = 2,				\
	.link_speed_capability = GEN_MAX,			\
	.turn_off_unused_lanes = true,			\
	.link_aspm = ASPM_L1,				\
	.link_aspm_L1_1 = true, \
	.link_aspm_L1_2 = true, \
	.link_hotplug = HOTPLUG_DISABLED,		\
	.clk_req = CLK_REQ1, \
	.port_params = {PP_PSPP_AC, 0x144, PP_PSPP_DC, 0x133}, \
}

#define strix_halo_ssd0_dxio_descriptor {				\
	.engine_type = PCIE_ENGINE,			\
	.port_present = true,				\
	.start_logical_lane = 12,				\
	.end_logical_lane = 15,					\
	.device_number = 2,				\
	.function_number = 5,				\
	.link_speed_capability = GEN_MAX,			\
	.turn_off_unused_lanes = true,			\
	.link_aspm = ASPM_L1,				\
	.link_aspm_L1_1 = true, \
	.link_aspm_L1_2 = true, \
	.link_hotplug = HOTPLUG_DISABLED,		\
	.clk_req = CLK_REQ2, \
	.port_params = {PP_PSPP_AC, 0x144, PP_PSPP_DC, 0x133}, \
}

#define strix_halo_wlan_dxio_descriptor {				\
	.engine_type = PCIE_ENGINE,			\
	.port_present = true,	\
	.start_logical_lane = 10,	\
	.end_logical_lane = CONFIG(ENABLE_WLANx2_WWANx0_MAPLE) ? 11 : 10,		\
	.device_number = 2,				\
	.function_number = 3,				\
	.link_speed_capability = GEN_MAX,			\
	.turn_off_unused_lanes = true,			\
	.link_aspm = ASPM_L1,				\
	.link_aspm_L1_1 = true, \
	.link_aspm_L1_2 = true, \
	.link_hotplug = HOTPLUG_DISABLED,		\
	.clk_req = CLK_REQ3,				\
}

#define strix_halo_wwan_dxio_descriptor {				\
	.engine_type = PCIE_ENGINE,			\
	.port_present = true,	\
	.start_logical_lane = 11,				\
	.end_logical_lane = CONFIG(ENABLE_WLANx0_WWANx2_MAPLE) ? 10 : 11,		\
	.device_number = 2,				\
	.function_number = 4,				\
	.link_speed_capability = GEN_MAX,			\
	.turn_off_unused_lanes = true,			\
	.link_aspm = ASPM_L1,				\
	.link_aspm_L1_1 = true, \
	.link_aspm_L1_2 = true, \
	.link_hotplug = HOTPLUG_DISABLED,		\
	.clk_req = CLK_REQ4,				\
	.port_params = {PP_PSPP_AC, 0x144, PP_PSPP_DC, 0x133}, \
}

#define strix_halo_gbe_dxio_descriptor {				\
	.engine_type = PCIE_ENGINE,			\
	.port_present = true,				\
	.start_logical_lane = 8,				\
	.end_logical_lane = 8,					\
	.device_number = 2,				\
	.function_number = 1,				\
	.link_speed_capability = GEN_MAX,			\
	.turn_off_unused_lanes = true,			\
	.link_aspm = ASPM_L1,				\
	.link_aspm_L1_1 = true, \
	.link_aspm_L1_2 = true, \
	.link_hotplug = HOTPLUG_DISABLED,		\
	.clk_req = CLK_REQ6,				\
	.port_params = {PP_PSPP_AC, 0x144, PP_PSPP_DC, 0x133}, \
}

#define strix_halo_sd_dxio_descriptor {				\
	.engine_type = PCIE_ENGINE,			\
	.port_present = true,				\
	.start_logical_lane = 9,				\
	.end_logical_lane = 9,					\
	.device_number = 2,				\
	.function_number = 2,				\
	.link_speed_capability = GEN_MAX,			\
	.turn_off_unused_lanes = true,			\
	.link_aspm = ASPM_L1,				\
	.link_aspm_L1_1 = true, \
	.link_aspm_L1_2 = true, \
	.link_hotplug = HOTPLUG_BASIC,		\
	.clk_req = CLK_REQ5,				\
	.port_params = {PP_PSPP_AC, 0x144, PP_PSPP_DC, 0x133}, \
}

static fsp_ddi_descriptor maple_strix_halo_ddi_descriptors[] = {
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

static uint8_t get_ddi1_type(void)
{
	const uint8_t eeprom_i2c_bus = 2;
	const uint8_t eeprom_i2c_address = 0x55;
	const uint16_t eeprom_connector_type_offset = 2;
	uint16_t connector_type = 0;

	if (i2c_2ba_read_bytes(eeprom_i2c_bus, eeprom_i2c_address,
			       eeprom_connector_type_offset, (void *)&connector_type,
			       sizeof(connector_type))) {
		printk(BIOS_NOTICE,
		       "Display connector type couldn't be determined. Disabling DDI1.\n");
		return DDI_UNUSED_TYPE;
	}

	/* On some NOVA cards it's LE, on some it's BE... */
	switch (connector_type) {
	case 0x0c00:
	case 0x000c:
		printk(BIOS_DEBUG, "Configuring DDI1 as HDMI.\n");
		return DDI_HDMI;
	case 0x1300:
	case 0x0013:
		printk(BIOS_DEBUG, "Configuring DDI1 as DP.\n");
		return DDI_DP;
	case 0x1400:
	case 0x0014:
		printk(BIOS_DEBUG, "Configuring DDI1 as eDP.\n");
		return DDI_EDP;
	case 0x1700:
	case 0x0017:
		printk(BIOS_DEBUG, "Configuring DDI1 as USB-C.\n");
		return DDI_DP_W_TYPEC;
	default:
		printk(BIOS_WARNING, "Unexpected display connector type %x. Disabling DDI1.\n",
		       connector_type);
		return DDI_UNUSED_TYPE;
	}
}

void mainboard_get_dxio_ddi_descriptors(
		const fsp_dxio_descriptor **dxio_descs, size_t *dxio_num,
		const fsp_ddi_descriptor **ddi_descs, size_t *ddi_num)
{
	maple_strix_halo_ddi_descriptors[1].connector_type = get_ddi1_type();

	static const fsp_dxio_descriptor maple_strix_halo_dxio_descriptors[] = {
		strix_halo_ssd1_dxio_descriptor,
		strix_halo_dt_mpcie_dxio_descriptor,
		strix_halo_ssd0_dxio_descriptor,
#if !CONFIG(ENABLE_WLANx0_WWANx2_MAPLE)
		strix_halo_wlan_dxio_descriptor,
#endif
#if !CONFIG(ENABLE_WLANx2_WWANx0_MAPLE)
		strix_halo_wwan_dxio_descriptor,
#endif
		strix_halo_gbe_dxio_descriptor,
		strix_halo_sd_dxio_descriptor,
	};

	*dxio_descs = maple_strix_halo_dxio_descriptors;
	*dxio_num = ARRAY_SIZE(maple_strix_halo_dxio_descriptors);
	*ddi_descs = maple_strix_halo_ddi_descriptors;
	*ddi_num = ARRAY_SIZE(maple_strix_halo_ddi_descriptors);
}
