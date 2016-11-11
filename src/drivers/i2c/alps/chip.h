/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corporation.
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

#ifndef __I2C_ALPS_CHIP_H__
#define __I2C_ALPS_CHIP_H__

#include <drivers/i2c/generic/chip.h>

#define ALPS0001_HID		"ALPS0001"
#define PNP0C50_CID		"PNP0C50"
#define ALPS_TP_DESC		"ALPS Touchpad"

struct drivers_i2c_alps_config {
	struct drivers_i2c_generic_config generic;
	uint8_t hid_desc_reg_offset;
};

#endif /* __I2C_ALPS_CHIP_H__ */
