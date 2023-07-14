/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/i2c_simple.h>
#include <gpio.h>
#include <soc/platform_descriptors.h>
#include <types.h>



static const fsp_dxio_descriptor mayan_dxio_descriptors[] = {
	{
		// MXM
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_lane = 0,
		.end_lane = 3,
		.device_number = 1,
		.function_number = 1,
		.link_speed_capability = GEN_MAX,
		.turn_off_unused_lanes = true,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.link_hotplug = HOTPLUG_DISABLED,
		.gpio_group_id = 4,
		.clk_pm_support = true,
		.clk_req = CLK_REQ0,
		.eq_preset = 3,
		.port_params = {PP_PSPP_AC, 0x144, PP_PSPP_DC, 0x133}
	},
	{
		// M2 SSD0-NVME
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_lane = 16,
		.end_lane = 19,
		.device_number = 2,
		.function_number = 4,
		.link_speed_capability = GEN_MAX,
		.turn_off_unused_lanes = true,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.link_hotplug = HOTPLUG_DISABLED,
		.gpio_group_id = 27,
		.clk_pm_support = true,
		.clk_req = CLK_REQ4,
		.eq_preset = 3,
		.port_params = {PP_PSPP_AC, 0x144, PP_PSPP_DC, 0x133}
	},
	{
		// X1
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_lane = 12,
		.end_lane = 12,
		.device_number = 1,
		.function_number = 3,
		.link_speed_capability = GEN_MAX,
		.turn_off_unused_lanes = true,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.link_hotplug = HOTPLUG_DISABLED,
		.clk_pm_support = true,
		.clk_req = CLK_REQ2,
		.eq_preset = 3,
		.port_params = {PP_PSPP_AC, 0x144, PP_PSPP_DC, 0x133}
	},
	{
		// DT
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_lane = 8,
		.end_lane = 9,
		.device_number = 1,
		.function_number = 2,
		.link_speed_capability = GEN_MAX,
		.turn_off_unused_lanes = true,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.link_hotplug = HOTPLUG_DISABLED,
		.clk_pm_support = true,
		.clk_req = CLK_REQ1,
		.eq_preset = 3,
		.port_params = {PP_PSPP_AC, 0x144, PP_PSPP_DC, 0x133}
	},
};

static fsp_ddi_descriptor mayan_ddi_descriptors[] = {
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
		.connector_type = DDI_DP,
		.aux_index = DDI_AUX3,
		.hdp_index = DDI_HDP3,
	},
	{ /* DDI3 - DP (type C) */
		.connector_type = DDI_DP,
		.aux_index = DDI_AUX4,
		.hdp_index = DDI_HDP4,
	},
	{ /* DDI4 - unused */
		.connector_type = DDI_UNUSED_TYPE,
		.aux_index = DDI_AUX5,
		.hdp_index = DDI_HDP5,
	}
};

static uint8_t get_ddi1_type(void)
{
	const uint8_t eeprom_i2c_bus = 2;
	const uint8_t eeprom_i2c_address = 0x55;
	const uint16_t eeprom_connector_type_offset = 2;
	uint8_t eeprom_connector_type_data[2];
	uint16_t connector_type;

	if (i2c_2ba_read_bytes(eeprom_i2c_bus, eeprom_i2c_address,
			       eeprom_connector_type_offset, eeprom_connector_type_data,
			       sizeof(eeprom_connector_type_data))) {
		printk(BIOS_NOTICE,
		       "Display connector type couldn't be determined. Disabling DDI1.\n");
		return DDI_UNUSED_TYPE;
	}

	connector_type = eeprom_connector_type_data[1] | eeprom_connector_type_data[0] << 8;

	switch (connector_type) {
	case 0xc:
		printk(BIOS_DEBUG, "Configuring DDI1 as HDMI.\n");
		return DDI_HDMI;
	case 0x13:
		printk(BIOS_DEBUG, "Configuring DDI1 as DP.\n");
		return DDI_DP;
	case 0x14:
		printk(BIOS_DEBUG, "Configuring DDI1 as eDP.\n");
		return DDI_EDP;
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
	mayan_ddi_descriptors[1].connector_type = get_ddi1_type();

	*dxio_descs = mayan_dxio_descriptors;
	*dxio_num = ARRAY_SIZE(mayan_dxio_descriptors);
	*ddi_descs = mayan_ddi_descriptors;
	*ddi_num = ARRAY_SIZE(mayan_ddi_descriptors);
}
