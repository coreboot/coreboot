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

#ifndef __DRIVERS_I2C_HID_CHIP_H__
#define __DRIVERS_I2C_HID_CHIP_H__

#include <drivers/i2c/generic/chip.h>

#define I2C_HID_CID	"PNP0C50"

struct drivers_i2c_hid_config {
	struct drivers_i2c_generic_config generic;
	uint8_t hid_desc_reg_offset;
};

#endif /* __I2C_HID_CHIP_H__ */
