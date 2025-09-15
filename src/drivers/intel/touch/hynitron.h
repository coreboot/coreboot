/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_TOUCH_DEV_HYNITRON_H__
#define __DRIVERS_TOUCH_DEV_HYNITRON_H__

#include "chip.h"

static const struct drivers_intel_touch_config hynitron_touch_config = {
	.sensor_dev_name = "Hynitron HFW68H Touch Pad Device",
	.dev_hidi2c = {
		.hid					= "HFW68H",
		.cid					= "PNP0C50",
		.intf.hidi2c.addr			= 0x2c,
		.intf.hidi2c.descriptor_address		= 0x20,
		.intf.hidi2c.connection_speed		= I2C_SPEED_FAST, /* fast mode */
	},
};

#endif /* __DRIVERS_TOUCH_DEV_HYNITRON_H__ */
