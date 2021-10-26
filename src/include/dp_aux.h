/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _DP_AUX_H_
#define _DP_AUX_H_

#include <types.h>

enum {
	EDID_LENGTH         = 128,
	EDID_I2C_ADDR       = 0x50,
	EDID_EXTENSION_FLAG = 0x7e,
};

enum i2c_over_aux {
	I2C_OVER_AUX_WRITE_MOT_0 = 0x0,
	I2C_OVER_AUX_READ_MOT_0 = 0x1,
	I2C_OVER_AUX_WRITE_STATUS_UPDATE_0 = 0x2,
	I2C_OVER_AUX_WRITE_MOT_1 = 0x4,
	I2C_OVER_AUX_READ_MOT_1 = 0x5,
	I2C_OVER_AUX_WRITE_STATUS_UPDATE_1 = 0x6,
	NATIVE_AUX_WRITE = 0x8,
	NATIVE_AUX_READ = 0x9,
};

enum aux_request {
	DPCD_READ,
	DPCD_WRITE,
	I2C_RAW_READ,
	I2C_RAW_WRITE,
	I2C_RAW_READ_AND_STOP,
	I2C_RAW_WRITE_AND_STOP,
};

/* Backlight configuration */
#define DP_BACKLIGHT_MODE_SET			0x721
#define DP_BACKLIGHT_CONTROL_MODE_MASK		0x3
#define DP_BACKLIGHT_CONTROL_MODE_DPCD		0x2
#define DP_DISPLAY_CONTROL_REGISTER		0x720
#define DP_BACKLIGHT_ENABLE			0x1
#define DP_BACKLIGHT_BRIGHTNESS_MSB		0x722

#define DP_AUX_MAX_PAYLOAD_BYTES	16


bool dp_aux_request_is_write(enum aux_request request);
enum i2c_over_aux dp_get_aux_cmd(enum aux_request request, uint32_t remaining_after_this);

#endif
