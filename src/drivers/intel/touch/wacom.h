/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_TOUCH_DEV_WACOM_H__
#define __DRIVERS_TOUCH_DEV_WACOM_H__

#include "chip.h"

static const struct drivers_intel_touch_config wacom_touch_config = {
	.sensor_dev_name = "Wacom Touch Sensor Device",
	.dev_hidspi = {
		.hid						= "WACOM530D",
		.cid						= "PNP0C51",
		.intf.hidspi.input_report_header_address	= 0x0,
		.intf.hidspi.input_report_body_address		= 0x1000,
		.intf.hidspi.output_report_address		= 0x1000,
		.intf.hidspi.read_opcode			= 0xb,
		.intf.hidspi.write_opcode			= 0x2
	},
};

#endif /* __DRIVERS_TOUCH_DEV_WACOM_H__ */
