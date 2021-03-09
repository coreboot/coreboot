/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <acpi/acpi.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <drivers/i2c/designware/dw_i2c.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/i2c.h>
#include <soc/i2c.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/southbridge.h>
#include "chip.h"

#if ENV_X86
static const uintptr_t i2c_bus_address[I2C_MASTER_DEV_COUNT + I2C_SLAVE_DEV_COUNT] = {
	0,
	0,
	APU_I2C2_BASE,
	APU_I2C3_BASE,
	APU_I2C4_BASE, /* Can only be used in slave mode */
};
#else
static uintptr_t i2c_bus_address[I2C_MASTER_DEV_COUNT + I2C_SLAVE_DEV_COUNT];
#endif

uintptr_t dw_i2c_base_address(unsigned int bus)
{
	if (bus >= ARRAY_SIZE(i2c_bus_address))
		return 0;

	return i2c_bus_address[bus];
}

#if !ENV_X86
void i2c_set_bar(unsigned int bus, uintptr_t bar)
{
	if (bus >= ARRAY_SIZE(i2c_bus_address)) {
		printk(BIOS_ERR, "Error: i2c index out of bounds: %u.", bus);
		return;
	}

	i2c_bus_address[bus] = bar;
}
#endif

const struct dw_i2c_bus_config *dw_i2c_get_soc_cfg(unsigned int bus)
{
	const struct soc_amd_picasso_config *config;

	if (bus >= ARRAY_SIZE(config->i2c))
		return NULL;

	/* config is not NULL; if it was, config_of_soc calls die() internally */
	config = config_of_soc();

	return &config->i2c[bus];
}

static const char *i2c_acpi_name(const struct device *dev)
{
	if ((uintptr_t)dev->path.mmio.addr == i2c_bus_address[2])
		return "I2C2";
	else if ((uintptr_t)dev->path.mmio.addr == i2c_bus_address[3])
		return "I2C3";
	else if ((uintptr_t)dev->path.mmio.addr == i2c_bus_address[4])
		return "I2C4";
	return NULL;
}

int dw_i2c_soc_dev_to_bus(const struct device *dev)
{
	if ((uintptr_t)dev->path.mmio.addr == i2c_bus_address[2])
		return 2;
	else if ((uintptr_t)dev->path.mmio.addr == i2c_bus_address[3])
		return 3;
	else if ((uintptr_t)dev->path.mmio.addr == i2c_bus_address[4])
		return 4;
	return -1;
}

__weak void mainboard_i2c_override(int bus, uint32_t *pad_settings) { }

static void dw_i2c_soc_init(bool is_early_init)
{
	size_t i;
	const struct soc_amd_picasso_config *config;
	uint32_t pad_ctrl;
	int misc_reg;

	/* config is not NULL; if it was, config_of_soc calls die() internally */
	config = config_of_soc();

	for (i = I2C_MASTER_START_INDEX; i < ARRAY_SIZE(config->i2c); i++) {
		const struct dw_i2c_bus_config *cfg  = &config->i2c[i];

		if (cfg->early_init != is_early_init)
			continue;

		if (dw_i2c_init(i, cfg)) {
			printk(BIOS_ERR, "Failed to init i2c bus %zd\n", i);
			continue;
		}

		misc_reg = MISC_I2C0_PAD_CTRL + sizeof(uint32_t) * i;
		pad_ctrl = misc_read32(misc_reg);

		pad_ctrl &= ~I2C_PAD_CTRL_NG_MASK;
		pad_ctrl |= I2C_PAD_CTRL_NG_NORMAL;

		pad_ctrl &= ~I2C_PAD_CTRL_RX_SEL_MASK;
		pad_ctrl |= I2C_PAD_CTRL_RX_SEL_3_3V;

		pad_ctrl &= ~I2C_PAD_CTRL_FALLSLEW_MASK;
		pad_ctrl |= cfg->speed == I2C_SPEED_STANDARD
				? I2C_PAD_CTRL_FALLSLEW_STD
				: I2C_PAD_CTRL_FALLSLEW_LOW;
		pad_ctrl |= I2C_PAD_CTRL_FALLSLEW_EN;

		mainboard_i2c_override(i, &pad_ctrl);
		misc_write32(misc_reg, pad_ctrl);
	}
}

void i2c_soc_early_init(void)
{
	dw_i2c_soc_init(true);
}

void i2c_soc_init(void)
{
	dw_i2c_soc_init(false);
}

struct device_operations picasso_i2c_mmio_ops = {
	/* TODO(teravest): Move I2C resource info here. */
	.read_resources = noop_read_resources,
	.set_resources = noop_set_resources,
	.scan_bus = scan_smbus,
	.acpi_name = i2c_acpi_name,
	.acpi_fill_ssdt = dw_i2c_acpi_fill_ssdt,
};
