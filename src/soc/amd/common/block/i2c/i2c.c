/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/gpio.h>
#include <amdblocks/gpio_defs.h>
#include <amdblocks/i2c.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/i2c.h>
#include <device/mmio.h>
#include <drivers/i2c/designware/dw_i2c.h>
#include <gpio.h>
#include <types.h>

#define MAX_PIN_COUNT 4

uintptr_t dw_i2c_base_address(unsigned int bus)
{
	size_t num_ctrlrs;
	const struct soc_i2c_ctrlr_info *ctrlr = soc_get_i2c_ctrlr_info(&num_ctrlrs);

	if (bus >= num_ctrlrs) {
		printk(BIOS_ERR, "Bus ID %u is >= number of I2C controllers %zu\n",
								bus, num_ctrlrs);
		return 0;
	}

	return ctrlr[bus].bar;
}

const struct dw_i2c_bus_config *dw_i2c_get_soc_cfg(unsigned int bus)
{
	size_t num_buses = 0;
	const struct dw_i2c_bus_config *cfg = soc_get_i2c_bus_config(&num_buses);

	if (bus >= num_buses) {
		printk(BIOS_ERR, "Bus ID %u is >= number of I2C buses %zu\n", bus, num_buses);
		return NULL;
	}

	return &cfg[bus];
}

static const char *i2c_acpi_name(const struct device *dev)
{
	size_t i;
	size_t num_ctrlrs;
	const struct soc_i2c_ctrlr_info *ctrlr = soc_get_i2c_ctrlr_info(&num_ctrlrs);

	if (!(uintptr_t)dev->path.mmio.addr)
		die("NULL MMIO address at %s\n", __func__);

	for (i = 0; i < num_ctrlrs; i++) {
		if ((uintptr_t)dev->path.mmio.addr == ctrlr[i].bar)
			return ctrlr[i].acpi_name;
	}
	printk(BIOS_ERR, "%s: Could not find %lu\n", __func__, (uintptr_t)dev->path.mmio.addr);
	return NULL;
}

int dw_i2c_soc_dev_to_bus(const struct device *dev)
{
	size_t i;
	size_t num_ctrlrs;
	const struct soc_i2c_ctrlr_info *ctrlr = soc_get_i2c_ctrlr_info(&num_ctrlrs);

	if (!(uintptr_t)dev->path.mmio.addr)
		die("NULL MMIO address at %s\n", __func__);

	for (i = 0; i < num_ctrlrs; i++) {
		if ((uintptr_t)dev->path.mmio.addr == ctrlr[i].bar)
			return i;
	}
	printk(BIOS_ERR, "%s: Could not find %lu\n", __func__, (uintptr_t)dev->path.mmio.addr);
	return -1;
}

static void dw_i2c_soc_init(bool is_early_init)
{
	unsigned int bus;
	size_t num_buses = 0, num_ctrlrs = 0;
	const struct dw_i2c_bus_config *cfg = soc_get_i2c_bus_config(&num_buses);
	const struct soc_i2c_ctrlr_info *ctrlr = soc_get_i2c_ctrlr_info(&num_ctrlrs);

	/* Ensure that the number of controllers in devicetree and SoC match. */
	assert(num_buses == num_ctrlrs);

	for (bus = 0; bus < num_buses; bus++, cfg++, ctrlr++) {
		/*
		 * Skip initialization when controller is in peripheral mode or base address
		 * is not configured or is not the expected stage to initialize.
		 */
		if (ctrlr->mode == I2C_PERIPHERAL_MODE || !ctrlr->bar ||
						cfg->early_init != is_early_init)
			continue;

		if (dw_i2c_init(bus, cfg) != CB_SUCCESS) {
			printk(BIOS_ERR, "Failed to init i2c bus %u\n", bus);
			continue;
		}

		soc_i2c_misc_init(bus, cfg);
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

static void i2c_read_resources(struct device *dev)
{
	mmio_resource(dev, 0, dev->path.mmio.addr / KiB, 4);
}

struct device_operations soc_amd_i2c_mmio_ops = {
	.read_resources = i2c_read_resources,
	.set_resources = noop_set_resources,
	.scan_bus = scan_smbus,
	.acpi_name = i2c_acpi_name,
	.acpi_fill_ssdt = dw_i2c_acpi_fill_ssdt,
	.ops_i2c_bus = &dw_i2c_bus_ops,
};

static void drive_scl(const struct soc_i2c_peripheral_reset_info *reset_info, int val)
{
	size_t j;

	for (j = 0; j < reset_info->num_pins; j++) {
		if (reset_info->i2c_scl_reset_mask & reset_info->i2c_scl[j].pin_mask)
			gpio_set(reset_info->i2c_scl[j].pin.gpio, val);
	}

	gpio_get(0); /* Flush posted write */
	/*
	 * TODO(b/183010197): 4usec gets 85KHz for 1 pin, 70KHz for 4 pins. Ensure this delay
	 * works fine for all SoCs and make this delay configurable if required.
	 */
	udelay(4);
}

void sb_reset_i2c_peripherals(const struct soc_i2c_peripheral_reset_info *reset_info)
{
	struct soc_amd_gpio_register_save save_table[MAX_PIN_COUNT];
	size_t i;

	if (!reset_info || !reset_info->i2c_scl || !reset_info->num_pins ||
						!reset_info->i2c_scl_reset_mask)
		return;

	assert(reset_info->num_pins <= MAX_PIN_COUNT);

	/* Save and reprogram I2C SCL pins */
	for (i = 0; i < reset_info->num_pins; i++) {
		/* To program I2C pins without destroying their programming, the registers
		   that will be changed need to be saved first */
		gpio_save_pin_registers(reset_info->i2c_scl[i].pin.gpio, &save_table[i]);
		/* Program SCL GPIO as output driven high */
		gpio_configure_pads(&reset_info->i2c_scl[i].pin, 1);
	}

	/*
	 * Toggle SCL back and forth 9 times under 100KHz. A single read is
	 * needed after the writes to force the posted write to complete.
	 */
	for (i = 0; i < 9; i++) {
		drive_scl(reset_info, 1);
		drive_scl(reset_info, 0);
	}

	/* Restore I2C pins. */
	for (i = 0; i < reset_info->num_pins; i++)
		gpio_restore_pin_registers(reset_info->i2c_scl[i].pin.gpio, &save_table[i]);
}
