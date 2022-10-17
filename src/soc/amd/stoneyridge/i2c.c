/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <amdblocks/i2c.h>
#include <soc/iomap.h>
#include <soc/i2c.h>
#include "chip.h"
#include <drivers/i2c/designware/dw_i2c.h>

/* Table to switch SCL pins to outputs to initially reset the I2C peripherals */
static const struct soc_i2c_scl_pin i2c_scl_pins[] = {
	I2C_RESET_SCL_PIN(I2C0_SCL_PIN, GPIO_I2C0_SCL),
	I2C_RESET_SCL_PIN(I2C1_SCL_PIN, GPIO_I2C1_SCL),
	I2C_RESET_SCL_PIN(I2C2_SCL_PIN, GPIO_I2C2_SCL),
	I2C_RESET_SCL_PIN(I2C3_SCL_PIN, GPIO_I2C3_SCL),
};

static const struct soc_i2c_ctrlr_info i2c_ctrlr[] = {
	{ I2C_MASTER_MODE, APU_I2C0_BASE, "I2CA" },
	{ I2C_MASTER_MODE, APU_I2C1_BASE, "I2CB" },
	{ I2C_MASTER_MODE, APU_I2C2_BASE, "I2CC" },
	{ I2C_MASTER_MODE, APU_I2C3_BASE, "I2CD" },
};

void reset_i2c_peripherals(void)
{
	const struct soc_amd_stoneyridge_config *cfg = config_of_soc();
	struct soc_i2c_peripheral_reset_info reset_info;

	reset_info.i2c_scl_reset_mask = cfg->i2c_scl_reset & GPIO_I2C_MASK;
	reset_info.i2c_scl = i2c_scl_pins;
	reset_info.num_pins = ARRAY_SIZE(i2c_scl_pins);
	sb_reset_i2c_peripherals(&reset_info);
}

const struct soc_i2c_ctrlr_info *soc_get_i2c_ctrlr_info(size_t *num_ctrlrs)
{
	*num_ctrlrs = ARRAY_SIZE(i2c_ctrlr);
	return i2c_ctrlr;
}

const struct dw_i2c_bus_config *soc_get_i2c_bus_config(size_t *num_buses)
{
	const struct soc_amd_stoneyridge_config *config = config_of_soc();

	*num_buses = ARRAY_SIZE(config->i2c);
	return config->i2c;
}

void soc_i2c_misc_init(unsigned int bus, const struct dw_i2c_bus_config *cfg)
{
	/* Do nothing. */
}
