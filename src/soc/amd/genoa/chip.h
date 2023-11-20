/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __GENOA_CHIP_H__
#define __GENOA_CHIP_H__

#include <amdblocks/chip.h>
#include <amdblocks/i2c.h>
#include <drivers/i2c/designware/dw_i2c.h>
#include <soc/iomap.h>
#include <types.h>

struct soc_amd_genoa_config {
	struct soc_amd_common_config common_config;

	u8 i2c_scl_reset;
	struct dw_i2c_bus_config i2c[I2C_CTRLR_COUNT];
};

#endif
