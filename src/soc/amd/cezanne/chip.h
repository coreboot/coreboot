/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CEZANNE_CHIP_H
#define CEZANNE_CHIP_H

#include <amdblocks/chip.h>
#include <soc/i2c.h>
#include <drivers/i2c/designware/dw_i2c.h>
#include <types.h>

struct soc_amd_cezanne_config {
	struct soc_amd_common_config common_config;
	u8 i2c_scl_reset;
	struct dw_i2c_bus_config i2c[I2C_CTRLR_COUNT];

	/* Enable S0iX support */
	bool s0ix_enable;

	enum {
		DOWNCORE_AUTO = 0,
		DOWNCORE_1 = 1, /* Run with 1 physical core */
		DOWNCORE_2 = 3, /* Run with 2 physical cores */
		DOWNCORE_3 = 4, /* Run with 3 physical cores */
		DOWNCORE_4 = 6, /* Run with 4 physical cores */
		DOWNCORE_5 = 8, /* Run with 5 physical cores */
		DOWNCORE_6 = 9, /* Run with 6 physical cores */
		DOWNCORE_7 = 10, /* Run with 7 physical cores */
	} downcore_mode;
	bool disable_smt; /* disable second thread on all physical cores */
};

#endif /* CEZANNE_CHIP_H */
