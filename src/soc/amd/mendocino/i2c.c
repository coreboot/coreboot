/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/i2c.h>
#include <console/console.h>
#include <soc/i2c.h>
#include <soc/southbridge.h>
#include "chip.h"

#if ENV_X86
static const struct soc_i2c_ctrlr_info i2c_ctrlr[I2C_CTRLR_COUNT] = {
	{ I2C_MASTER_MODE, APU_I2C0_BASE, "I2C0" },
	{ I2C_MASTER_MODE, APU_I2C1_BASE, "I2C1" },
	{ I2C_MASTER_MODE, APU_I2C2_BASE, "I2C2" },
	{ I2C_MASTER_MODE, APU_I2C3_BASE, "I2C3" }
};
#else
static struct soc_i2c_ctrlr_info i2c_ctrlr[I2C_CTRLR_COUNT] = {
	{ I2C_MASTER_MODE, 0, "" },
	{ I2C_MASTER_MODE, 0, "" },
	{ I2C_MASTER_MODE, 0, "" },
	{ I2C_MASTER_MODE, 0, "" }
};

void i2c_set_bar(unsigned int bus, uintptr_t bar)
{
	if (bus >= ARRAY_SIZE(i2c_ctrlr)) {
		printk(BIOS_ERR, "Error: i2c index out of bounds: %u.", bus);
		return;
	}

	i2c_ctrlr[bus].bar = bar;
}
#endif

void soc_i2c_misc_init(unsigned int bus, const struct dw_i2c_bus_config *cfg)
{
	const struct soc_amd_mendocino_config *config = config_of_soc();

	if (bus >= ARRAY_SIZE(config->i2c_pad))
		return;

	fch_i23c_pad_init(bus, cfg->speed, &config->i2c_pad[bus]);
}

const struct soc_i2c_ctrlr_info *soc_get_i2c_ctrlr_info(size_t *num_ctrlrs)
{
	*num_ctrlrs = ARRAY_SIZE(i2c_ctrlr);
	return i2c_ctrlr;
}

const struct dw_i2c_bus_config *soc_get_i2c_bus_config(size_t *num_buses)
{
	const struct soc_amd_mendocino_config *config = config_of_soc();

	*num_buses = ARRAY_SIZE(config->i2c);
	return config->i2c;
}
