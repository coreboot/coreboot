/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_I2C_HID_CHIP_H__
#define __DRIVERS_I2C_HID_CHIP_H__

#include <drivers/i2c/generic/chip.h>

#define I2C_HID_CID	"PNP0C50"

struct drivers_i2c_hid_config {
	struct drivers_i2c_generic_config generic;
	uint8_t hid_desc_reg_offset;
};

#endif /* __I2C_HID_CHIP_H__ */
