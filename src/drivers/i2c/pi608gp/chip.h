/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_I2C_PI608GP_CHIP_H__
#define __DRIVERS_I2C_PI608GP_CHIP_H__

#include "pi608gp.h"

struct drivers_i2c_pi608gp_config {
	bool gen2_3p5_enable;
	uint32_t gen2_3p5_amp;
	struct deemph_lvl gen2_3p5_deemph;
};

#endif /* __DRIVERS_I2C_PI608GP_CHIP_H__ */
