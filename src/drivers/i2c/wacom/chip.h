/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __I2C_WACOM_CHIP_H__
#define __I2C_WACOM_CHIP_H__

#include <drivers/i2c/generic/chip.h>

#define WCOM50C1_HID		"WCOM50C1"
#define WCOMNTN2_HID		"WCOMNTN2"
#define PNP0C50_CID		"PNP0C50"
#define WCOM_TS_DESC		"WCOM Touchscreen"
#define WCOM_DT_DESC		"WCOM Digitizer"

struct drivers_i2c_wacom_config {
	struct drivers_i2c_generic_config generic;
	uint8_t hid_desc_reg_offset;
};

#endif /* __I2C_WACOM_CHIP_H__ */
