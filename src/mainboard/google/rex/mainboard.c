/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <device/device.h>
#include <drivers/wwan/fm/chip.h>
#include <ec/ec.h>
#include <fw_config.h>
#include <soc/ramstage.h>
#include <static.h>
#include <stdio.h>
#include <stdlib.h>
#include <vendorcode/google/chromeos/chromeos.h>

WEAK_DEV_PTR(rp6_wwan);

void __weak fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	/* default implementation does nothing */
}

void mainboard_update_soc_chip_config(struct soc_intel_meteorlake_config *config)
{
	variant_update_soc_chip_config(config);
}

__weak void variant_update_soc_chip_config(struct soc_intel_meteorlake_config *config)
{
	/* default implementation does nothing */
}

static void mainboard_init(void *chip_info)
{
	struct pad_config *padbased_table;
	const struct pad_config *base_pads;
	size_t base_num;

	padbased_table = new_padbased_table();
	base_pads = variant_gpio_table(&base_num);
	gpio_padbased_override(padbased_table, base_pads, base_num);
	fw_config_gpio_padbased_override(padbased_table);
	gpio_configure_pads_with_padbased(padbased_table);
	free(padbased_table);
	baseboard_devtree_update();
}

void __weak baseboard_devtree_update(void)
{
	/* Override dev tree settings per baseboard */
}

void __weak variant_generate_s0ix_hook(enum s0ix_entry entry)
{
	/* Add board-specific MS0X entries */
	/*
	   if (s0ix_entry == S0IX_ENTRY) {
		implement variant operations here
	   }
	   if (s0ix_entry == S0IX_EXIT) {
		implement variant operations here
	   }
	 */
}

static void mainboard_generate_s0ix_hook(void)
{
	acpigen_write_if_lequal_op_int(ARG0_OP, 1);
	{
		if (CONFIG(HAVE_SLP_S0_GATE))
			acpigen_soc_clear_tx_gpio(GPIO_SLP_S0_GATE);
		variant_generate_s0ix_hook(S0IX_ENTRY);
	}
	acpigen_write_else();
	{
		if (CONFIG(HAVE_SLP_S0_GATE))
			acpigen_soc_set_tx_gpio(GPIO_SLP_S0_GATE);
		variant_generate_s0ix_hook(S0IX_EXIT);
	}
	acpigen_write_if_end();
}

static void mainboard_generate_wwan_shutdown(const struct device *dev)
{
	const struct drivers_wwan_fm_config *config = config_of(dev);
	const struct device *parent = dev->upstream->dev;

	if (!config)
		return;
	if (config->rtd3dev) {
		acpigen_write_store();
		acpigen_emit_namestring(acpi_device_path_join(parent, "RTD3._STA"));
		acpigen_emit_byte(LOCAL0_OP);
		acpigen_write_if_lequal_op_int(LOCAL0_OP, ONE_OP);
		{
			acpigen_emit_namestring(acpi_device_path_join(dev, "DPTS"));
			acpigen_emit_byte(ARG0_OP);
		}
		acpigen_write_if_end();
	}
}

static void mainboard_fill_ssdt(const struct device *dev)
{
	const struct device *wwan = DEV_PTR(rp6_wwan);

	if (is_dev_enabled(wwan)) {
		acpigen_write_scope("\\_SB");
		acpigen_write_method_serialized("MPTS", 1);
		mainboard_generate_wwan_shutdown(wwan);
		acpigen_write_method_end(); /* Method */
		acpigen_write_scope_end(); /* Scope */
	}

	acpigen_write_scope("\\_SB");
	acpigen_write_method_serialized("MS0X", 1);
	mainboard_generate_s0ix_hook();
	acpigen_write_method_end(); /* Method */
	acpigen_write_scope_end(); /* Scope */
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

static void mainboard_dev_init(struct device *dev)
{
	mainboard_ec_init();
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_dev_init;
	dev->ops->get_smbios_strings = mainboard_smbios_strings;
	dev->ops->acpi_fill_ssdt = mainboard_fill_ssdt;
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};
