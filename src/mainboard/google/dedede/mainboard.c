/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <baseboard/variants.h>
#include <device/device.h>
#include <drivers/spi/tpm/tpm.h>
#include <ec/ec.h>
#include <security/tpm/tss.h>
#include <soc/soc_chip.h>
#include <vendorcode/google/chromeos/chromeos.h>

static void mainboard_update_soc_chip_config(void)
{
	struct soc_intel_jasperlake_config *cfg = config_of_soc();
	int ret;

	ret = tlcl_lib_init();
	if (ret != VB2_SUCCESS) {
		printk(BIOS_ERR, "tlcl_lib_init() failed: 0x%x\n", ret);
		return;
	}

	if (!cr50_is_long_interrupt_pulse_enabled()) {
		/* Disable GPIO PM to allow for shorter IRQ pulses */
		printk(BIOS_INFO, "Override GPIO PM\n");
		cfg->gpio_override_pm = 1;
		memset(cfg->gpio_pm, 0, sizeof(cfg->gpio_pm));
	}
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

	mainboard_update_soc_chip_config();
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
