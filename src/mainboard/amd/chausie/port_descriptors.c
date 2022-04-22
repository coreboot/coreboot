/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/i2c_simple.h>
#include <soc/gpio.h>
#include <soc/platform_descriptors.h>
#include <types.h>

static const fsp_dxio_descriptor chausie_dxio_descriptors[] = {
	{ /* GBE*/
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 0,
		.end_logical_lane = 0,
		.device_number = 2,
		.function_number = 1,
		.link_speed_capability = GEN3,
		.turn_off_unused_lanes = true,
		.link_aspm = 2,
		.link_hotplug = 3,
		.clk_req = CLK_REQ3,
	},
	{ /* WIFI */
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 1,
		.end_logical_lane = 1,
		.device_number = 2,
		.function_number = 2,
		.link_speed_capability = GEN3,
		.turn_off_unused_lanes = true,
		.link_aspm = 2,
		.link_hotplug = 3,
		.clk_req = CLK_REQ1,
	},
	{ /* NVMe SSD */
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 2,
		.end_logical_lane = 3,
		.device_number = 2,
		.function_number = 3,
		.link_speed_capability = GEN3,
		.turn_off_unused_lanes = true,
		.link_aspm = 2,
		.link_hotplug = 3,
		.gpio_group_id = GPIO_27,
		.clk_req = CLK_REQ0,
	},
};

static fsp_ddi_descriptor chausie_ddi_descriptors[] = {
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
		break;
	case 0x13:
		printk(BIOS_DEBUG, "Configuring DDI1 as DP.\n");
		return DDI_DP;
		break;
	case 0x14:
		printk(BIOS_DEBUG, "Configuring DDI1 as eDP.\n");
		return DDI_EDP;
		break;
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
	chausie_ddi_descriptors[1].connector_type = get_ddi1_type();

	*dxio_descs = chausie_dxio_descriptors;
	*dxio_num = ARRAY_SIZE(chausie_dxio_descriptors);
	*ddi_descs = chausie_ddi_descriptors;
	*ddi_num = ARRAY_SIZE(chausie_ddi_descriptors);
}
