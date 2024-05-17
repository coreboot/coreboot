/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/i2c_simple.h>
#if CONFIG(PLATFORM_USES_FSP2_0)
#include <soc/platform_descriptors.h>
#else
#include <soc/amd/phoenix/chip_opensil.h>
#endif
#include <types.h>
#include "display_card_type.h"

uint8_t get_ddi1_type(void)
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
	case 0x0c:
		printk(BIOS_DEBUG, "Configuring DDI1 as HDMI.\n");
		return DDI_HDMI;
	case 0x13:
		printk(BIOS_DEBUG, "Configuring DDI1 as DP.\n");
		return DDI_DP;
	case 0x14:
		printk(BIOS_DEBUG, "Configuring DDI1 as eDP.\n");
		return DDI_EDP;
	case 0x17:
		printk(BIOS_DEBUG, "Configuring DDI1 as USB-C.\n");
		return DDI_DP_W_TYPEC;
	default:
		printk(BIOS_WARNING, "Unexpected display connector type %x. Disabling DDI1.\n",
		       connector_type);
		return DDI_UNUSED_TYPE;
	}
}
