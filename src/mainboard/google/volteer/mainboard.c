/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <acpi/acpi.h>
#include <baseboard/variants.h>
#include <device/device.h>
#include <drivers/spi/tpm/tpm.h>
#include <ec/ec.h>
#include <fw_config.h>
#include <security/tpm/tss.h>
#include <soc/gpio.h>
#include <soc/ramstage.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <variant/gpio.h>
#include <vb2_api.h>

static void mainboard_init(struct device *dev)
{
	mainboard_ec_init();
}

static void add_fw_config_oem_string(const struct fw_config *config, void *arg)
{
	struct smbios_type11 *t;
	char buffer[64];

	t = (struct smbios_type11 *)arg;

	snprintf(buffer, sizeof(buffer), "%s-%s", config->field_name, config->option_name);
	t->count = smbios_add_string(t->eos, buffer);
}

static void mainboard_smbios_strings(struct device *dev, struct smbios_type11 *t)
{
	fw_config_for_each_found(add_fw_config_oem_string, t);
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_init;
	dev->ops->acpi_inject_dsdt = chromeos_dsdt_generator;
	dev->ops->get_smbios_strings = mainboard_smbios_strings;
}

void mainboard_update_soc_chip_config(struct soc_intel_tigerlake_config *cfg)
{
	int ret;
	ret = tlcl_lib_init();
	if (ret != VB2_SUCCESS) {
		printk(BIOS_ERR, "tlcl_lib_init() failed: 0x%x\n", ret);
		return;
	}

	if (cr50_is_long_interrupt_pulse_enabled()) {
		printk(BIOS_INFO, "Enabling S0i3.4\n");
	} else {
		/*
		 * Disable S0i3.4, preventing the GPIO block from switching to
		 * slow clock.
		 */
		printk(BIOS_INFO, "Not enabling S0i3.4\n");
		cfg->LpmStateDisableMask |= LPM_S0i3_4;
		cfg->gpio_override_pm = 1;
		memset(cfg->gpio_pm, 0, sizeof(cfg->gpio_pm));
	}
}

static void mainboard_chip_init(void *chip_info)
{
	const struct pad_config *base_pads;
	const struct pad_config *override_pads;
	size_t base_num, override_num;

	base_pads = variant_base_gpio_table(&base_num);
	override_pads = variant_override_gpio_table(&override_num);

	gpio_configure_pads_with_override(base_pads, base_num,
		override_pads, override_num);
}

struct chip_operations mainboard_ops = {
	.init = mainboard_chip_init,
	.enable_dev = mainboard_enable,
};
