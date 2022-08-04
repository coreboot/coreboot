/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <console/console.h>
#include <device/device.h>
#include <device/path.h>
#include <device/spi.h>
#include <spi-generic.h>
#include <string.h>
#include "chip.h"

static int spi_acpi_get_bus(const struct device *dev)
{
	struct device *spi_dev;
	struct device_operations *ops;

	if (!dev->bus || !dev->bus->dev)
		return -1;

	spi_dev = dev->bus->dev;
	ops = spi_dev->ops;

	if (ops && ops->ops_spi_bus && ops->ops_spi_bus->dev_to_bus)
		return ops->ops_spi_bus->dev_to_bus(spi_dev);

	return -1;
}

static int write_gpio(struct acpi_gpio *gpio, int *curr_index)
{
	int ret = -1;

	if (gpio->pin_count == 0)
		return ret;

	acpi_device_write_gpio(gpio);
	ret = *curr_index;
	(*curr_index)++;

	return ret;
}

static void nxp_uwb_fill_ssdt(const struct device *dev)
{
	struct drivers_nxp_uwb_config *config = dev->chip_info;
	const char *scope = acpi_device_scope(dev);
	const char *path = acpi_device_path(dev);
	struct acpi_spi spi = {
		.device_select = dev->path.spi.cs,
		.speed = config->speed ? : 1 * MHz,
		.resource = scope,
		.device_select_polarity = SPI_POLARITY_LOW,
		.wire_mode = SPI_4_WIRE_MODE,
		.data_bit_length = 8,
		.clock_phase = SPI_CLOCK_PHASE_FIRST,
		.clock_polarity = SPI_POLARITY_LOW,
	};
	int curr_index = 0;
	int irq_gpio_index = -1;
	int ce_gpio_index = -1;
	int ri_gpio_index = -1;

	if (!scope)
		return;

	if (spi_acpi_get_bus(dev) == -1) {
		printk(BIOS_ERR, "%s: Cannot get bus for device.\n",
		       dev_path(dev));
		return;
	}

	/* Device */
	acpigen_write_scope(scope);
	acpigen_write_device(acpi_device_name(dev));
	acpigen_write_name_string("_HID", ACPI_DT_NAMESPACE_HID);
	acpigen_write_name_integer("_UID", config->uid);
	if (config->desc)
		acpigen_write_name_string("_DDN", config->desc);
	acpigen_write_STA(acpi_device_status(dev));

	/* Resources */
	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();
	acpi_device_write_spi(&spi);
	irq_gpio_index = write_gpio(&config->irq_gpio, &curr_index);
	ce_gpio_index = write_gpio(&config->ce_gpio, &curr_index);
	ri_gpio_index = write_gpio(&config->ri_gpio, &curr_index);
	acpigen_write_resourcetemplate_footer();

	struct acpi_dp *dsd = acpi_dp_new_table("_DSD");
	acpi_dp_add_string(dsd, "compatible", "nxp,sr1xx");
	acpi_dp_add_gpio(dsd, "nxp,sr1xx-irq-gpios", path, irq_gpio_index, 0,
			 config->irq_gpio.active_low);

	acpi_dp_add_gpio(dsd, "nxp,sr1xx-ce-gpios", path, ce_gpio_index, 0,
			 config->ce_gpio.active_low);

	acpi_dp_add_gpio(dsd, "nxp,sr1xx-ri-gpios", path, ri_gpio_index, 0,
			 config->ri_gpio.active_low);
	acpi_dp_write(dsd);

	acpigen_write_device_end();
	acpigen_write_scope_end();

	printk(BIOS_INFO, "%s: %s at %s\n", path,
	       config->desc ? : dev->chip_ops->name, dev_path(dev));
}

static const char *nxp_uwb_name(const struct device *dev)
{
	struct drivers_nxp_uwb_config *config = dev->chip_info;
	static char name[ACPI_NAME_BUFFER_SIZE];

	if (config->name)
		snprintf(name, sizeof(name), "%s", config->name);
	else
		snprintf(name, sizeof(name), "UWB%1X", spi_acpi_get_bus(dev));
	name[4] = '\0';
	return name;
}

static struct device_operations nxp_uwb_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= nxp_uwb_name,
	.acpi_fill_ssdt		= nxp_uwb_fill_ssdt,
};

static void nxb_uwb_enable(struct device *dev)
{
	dev->ops = &nxp_uwb_ops;
}

struct chip_operations drivers_nxp_uwb_ops = {
	CHIP_NAME("NXP UWB Device")
	.enable_dev = nxb_uwb_enable
};
