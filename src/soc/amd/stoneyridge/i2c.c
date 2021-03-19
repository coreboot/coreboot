/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <amdblocks/i2c.h>
#include <soc/iomap.h>
#include <soc/i2c.h>
#include "chip.h"

static const struct soc_i2c_ctrlr_info i2c_ctrlr[] = {
	{ I2C_MASTER_MODE, I2CA_BASE_ADDRESS, "I2CA" },
	{ I2C_MASTER_MODE, I2CB_BASE_ADDRESS, "I2CB" },
	{ I2C_MASTER_MODE, I2CC_BASE_ADDRESS, "I2CC" },
	{ I2C_MASTER_MODE, I2CD_BASE_ADDRESS, "I2CD" },
};

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
