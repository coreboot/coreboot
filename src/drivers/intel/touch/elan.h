/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_TOUCH_DEV_ELAN_H__
#define __DRIVERS_TOUCH_DEV_ELAN_H__

#include "chip.h"

/*
 * BOM36 is used for HID-SPI interface, while BOM37A is used for HID-I2C
 * interface.
 */

static const struct drivers_intel_touch_config elan_touch_config = {
	.sensor_dev_name = "ELAN Touch Sensor Device",
	.dev_hidi2c = {	/* BOM37A */
		.hid				= "ELAN9048",
		.cid				= "PNP0C50",
		.intf.hidi2c.addr		= 0x16,
		.intf.hidi2c.descriptor_address	= 0x1,
		.intf.hidi2c.connection_speed	= I2C_SPEED_FAST, /* fast mode */
	},
	.dev_hidspi = { /* BOM36 */
		.hid						= "ELAN904A",
		.cid						= "PNP0C51",
		.intf.hidspi.input_report_header_address	= 0x1000,
		.intf.hidspi.input_report_body_address		= 0x1100,
		.intf.hidspi.output_report_address		= 0x2000,
		.intf.hidspi.read_opcode			= 0xb,
		.intf.hidspi.write_opcode			= 0x2,
	},
};

/*
 * ELAN9006 is used for HID-SPI interface, while ELAN6918 is used for HID-I2C
 * interface.
 */

static const struct drivers_intel_touch_config google_touch_config = {
	.sensor_dev_name = "ELAN Touch Sensor Device",
	.dev_hidi2c = {	/* Google's I2C-based touch */
		.hid				= "ELAN6918",
		.cid				= "PNP0C50",
		.intf.hidi2c.addr		= 0x10,
		.intf.hidi2c.descriptor_address	= 0x1,
		.intf.hidi2c.connection_speed	= I2C_SPEED_FAST, /* fast mode */
	},
	.dev_hidspi = { /* Google's SPI-based touch */
		.hid						= "ELAN9006",
		.cid						= "PNP0C51",
		.intf.hidspi.connection_speed			= (32 * MHz), /* unit: Hz */
		.intf.hidspi.input_report_header_address	= 0x1000,
		.intf.hidspi.input_report_body_address		= 0x1100,
		.intf.hidspi.output_report_address		= 0x2000,
		.intf.hidspi.read_opcode			= 0xb,
		.intf.hidspi.write_opcode			= 0x2,
	},
};

#endif /* __DRIVERS_TOUCH_DEV_ELAN_H__ */
