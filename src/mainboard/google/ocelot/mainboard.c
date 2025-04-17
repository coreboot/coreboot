/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <bootstate.h>
#include <device/device.h>
#include <ec/ec.h>
#include <soc/ramstage.h>
#include <stdlib.h>
#include <vendorcode/google/chromeos/chromeos.h>

void __weak fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	/* default implementation does nothing */
}

void mainboard_update_soc_chip_config(struct soc_intel_pantherlake_config *config)
{
	variant_update_soc_chip_config(config);
}

void __weak variant_update_soc_chip_config(struct soc_intel_pantherlake_config *config)
{
	/* default implementation does nothing */
}

static void mainboard_init(void *chip_info)
{
	baseboard_devtree_update();
}

/* Must happen before MPinit */
static void mainboard_early(void *unused)
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
}

BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_EXIT, mainboard_early, NULL);

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

static void mainboard_fill_ssdt(const struct device *dev)
{
	acpigen_write_scope("\\_SB");
	acpigen_write_method_serialized("MS0X", 1);
	mainboard_generate_s0ix_hook();
	acpigen_write_method_end(); /* Method */
	acpigen_write_scope_end(); /* Scope */
}

static void mainboard_dev_init(struct device *dev)
{
	mainboard_ec_init();
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_dev_init;
	dev->ops->acpi_fill_ssdt = mainboard_fill_ssdt;
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};
