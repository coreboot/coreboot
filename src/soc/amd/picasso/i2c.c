/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/i2c.h>
#include <soc/i2c.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/southbridge.h>
#include "chip.h"

#if ENV_X86
/* Preferably keep all the I2C controllers operating in a specific mode together. */
static const struct soc_i2c_ctrlr_info i2c_ctrlr[I2C_CTRLR_COUNT] = {
	{ I2C_MASTER_MODE, 0, "" },
	{ I2C_MASTER_MODE, 0, "" },
	{ I2C_MASTER_MODE, APU_I2C2_BASE, "I2C2" },
	{ I2C_MASTER_MODE, APU_I2C3_BASE, "I2C3" },
	{ I2C_PERIPHERAL_MODE, APU_I2C4_BASE, "I2C4" } /* Can only be used in peripheral mode */
};
#else
static struct soc_i2c_ctrlr_info i2c_ctrlr[I2C_CTRLR_COUNT] = {
	{ I2C_MASTER_MODE, 0, ""},
	{ I2C_MASTER_MODE, 0, "" },
	{ I2C_MASTER_MODE, 0, "" },
	{ I2C_MASTER_MODE, 0, "" },
	{ I2C_PERIPHERAL_MODE, 0, "" },
};

void i2c_set_bar(unsigned int bus, uintptr_t bar)
{
	if (bus >= ARRAY_SIZE(i2c_ctrlr)) {
		printk(BIOS_ERR, "i2c index out of bounds: %u.", bus);
		return;
	}

	i2c_ctrlr[bus].bar = bar;
}
#endif

void soc_i2c_misc_init(unsigned int bus, const struct dw_i2c_bus_config *cfg)
{
	/* TODO: Picasso supports I2C RX pad configurations 3.3V, 1.8V and off, so make this
	   configurable. */
	const struct i2c_pad_control ctrl = {
		.rx_level = I2C_PAD_RX_3_3V,
	};

	fch_i2c_pad_init(bus, cfg->speed, &ctrl);
}

const struct soc_i2c_ctrlr_info *soc_get_i2c_ctrlr_info(size_t *num_ctrlrs)
{
	*num_ctrlrs = ARRAY_SIZE(i2c_ctrlr);
	return i2c_ctrlr;
}

const struct dw_i2c_bus_config *soc_get_i2c_bus_config(size_t *num_buses)
{
	const struct soc_amd_picasso_config *config = config_of_soc();

	*num_buses = ARRAY_SIZE(config->i2c);
	return config->i2c;
}
