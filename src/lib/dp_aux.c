/* SPDX-License-Identifier: GPL-2.0-only */

#include <delay.h>
#include <dp_aux.h>
#include <console/console.h>
#include <timer.h>

bool dp_aux_request_is_write(enum aux_request request)
{
	switch (request) {
	case I2C_RAW_WRITE_AND_STOP:
	case I2C_RAW_WRITE:
	case DPCD_WRITE:
		return true;
	default:
		return false;
	}
}

enum i2c_over_aux dp_get_aux_cmd(enum aux_request request, uint32_t remaining_after_this)
{
	switch (request) {
	case I2C_RAW_WRITE_AND_STOP:
		if (!remaining_after_this)
			return I2C_OVER_AUX_WRITE_MOT_0;
		/* fallthrough */
	case I2C_RAW_WRITE:
		return I2C_OVER_AUX_WRITE_MOT_1;
	case I2C_RAW_READ_AND_STOP:
		if (!remaining_after_this)
			return I2C_OVER_AUX_READ_MOT_0;
		/* fallthrough */
	case I2C_RAW_READ:
		return I2C_OVER_AUX_READ_MOT_1;
	case DPCD_WRITE:
		return NATIVE_AUX_WRITE;
	case DPCD_READ:
	default:
		return NATIVE_AUX_READ;
	}
}
