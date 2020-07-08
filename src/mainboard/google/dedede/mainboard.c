/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <bootstate.h>
#include <baseboard/variants.h>
#include <device/device.h>
#include <ec/ec.h>
#include <vendorcode/google/chromeos/chromeos.h>

__weak void variant_isst_override(void)
{
	/*
	 * Implement the override only if the board uses very early/initial revisions of
	 * Silicon. Otherwise nothing to override.
	 */
}

static void mainboard_config_isst(void *unused)
{
	variant_isst_override();
}

static void mainboard_init(void *chip_info)
{
	const struct pad_config *base_pads;
	const struct pad_config *override_pads;
	size_t base_num, override_num;

	base_pads = variant_base_gpio_table(&base_num);
	override_pads = variant_override_gpio_table(&override_num);

	gpio_configure_pads_with_override(base_pads, base_num,
		override_pads, override_num);
}

static void mainboard_dev_init(struct device *dev)
{
	mainboard_ec_init();
}

static unsigned long mainboard_write_acpi_tables(
		const struct device *device, unsigned long current, acpi_rsdp_t *rsdp)
{
	return current;
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_dev_init;
	dev->ops->write_acpi_tables = mainboard_write_acpi_tables;
	dev->ops->acpi_inject_dsdt = chromeos_dsdt_generator;
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};

/* Configure ISST before CPU initialization */
BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_ENTRY, mainboard_config_isst, NULL);
