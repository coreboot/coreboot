/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CEZANNE_CHIP_H
#define CEZANNE_CHIP_H

#include <amdblocks/chip.h>
#include <soc/i2c.h>
#include <drivers/i2c/designware/dw_i2c.h>

struct soc_amd_cezanne_config {
	struct soc_amd_common_config common_config;
	u8 i2c_scl_reset;
	struct dw_i2c_bus_config i2c[I2C_CTRLR_COUNT];
};

#endif /* CEZANNE_CHIP_H */
