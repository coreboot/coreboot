/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <device/device.h>
#include <stdio.h>

#include "chip.h"
#include <console/console.h>

#define AMD_I2S_ACPI_DESC	"I2S machine driver"

static void i2s_machine_dev_fill_crs_dsd(const char *path,
					const struct acpi_gpio *dmic_select_gpio)
{
	struct acpi_dp *dsd;

	/* Resources */
	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();
	acpi_device_write_gpio(dmic_select_gpio);
	acpigen_write_resourcetemplate_footer();

	dsd = acpi_dp_new_table("_DSD");
	/*
	 * This GPIO is used to select DMIC0 or DMIC1 by the kernel driver. It does not
	 * really have a polarity since low and high control the selection of DMIC and
	 * hence does not have an active polarity.
	 * Kernel driver does not use the polarity field and instead treats the GPIO
	 * selection as follows:
	 * Set low (0) = Select DMIC0
	 * Set high (1) = Select DMIC1
	 */
	acpi_dp_add_gpio(dsd, "dmic-gpios", path,
			 0,  /* Index = 0 (There is a single GPIO entry in _CRS). */
			 0,  /* Pin = 0 (There is a single pin in the GPIO resource). */
			 0); /* Active low = 0 (Kernel driver does not use active polarity). */
	acpi_dp_write(dsd);
}

static void i2s_machine_dev_fill_ssdt(const struct device *dev)
{
	const char *scope = acpi_device_scope(dev);
	const struct acpi_gpio *dmic_select_gpio;
	const struct drivers_amd_i2s_machine_dev_config *cfg;
	const char *path = acpi_device_path(dev);

	cfg = config_of(dev);

	dmic_select_gpio = &cfg->dmic_select_gpio;

	if (scope == NULL) {
		printk(BIOS_ERR, "%s: ERROR: ACPI I2S scope not found\n", dev_path(dev));
		return;
	}

	if (cfg->hid == NULL) {
		printk(BIOS_ERR, "%s: ERROR: HID required\n", dev_path(dev));
		return;
	}

	acpigen_write_scope(scope); /* Scope */
	acpigen_write_device(acpi_device_name(dev)); /* Device */
	acpigen_write_name_string("_HID", cfg->hid);
	acpigen_write_name_integer("_UID", cfg->uid);
	acpigen_write_name_string("_DDN", AMD_I2S_ACPI_DESC);

	acpigen_write_STA(acpi_device_status(dev));

	if (dmic_select_gpio->pin_count)
		i2s_machine_dev_fill_crs_dsd(path, dmic_select_gpio);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s: %s at %s\n", path, AMD_I2S_ACPI_DESC, dev_path(dev));
}

static const char *i2s_machine_dev_acpi_name(const struct device *dev)
{
	static char name[5];
	snprintf(name, sizeof(name), "I2S%X", dev->path.generic.id);
	return name;
}

static struct device_operations i2s_machine_dev_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= i2s_machine_dev_acpi_name,
	.acpi_fill_ssdt		= i2s_machine_dev_fill_ssdt,
};

static void i2s_machine_dev_enable(struct device *dev)
{
	dev->ops = &i2s_machine_dev_ops;
}

struct chip_operations drivers_amd_i2s_machine_dev_ops = {
	.name = "AMD I2S Machine Device",
	.enable_dev = i2s_machine_dev_enable
};
