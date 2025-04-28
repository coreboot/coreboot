/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <chip.h>
#include <fw_config.h>
#include <soc/gpio_soc_defs.h>
#include <intelblocks/graphics.h>
#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <variant/gpio.h>


void variant_generate_s0ix_hook(enum s0ix_entry entry)
{
	if (entry == S0IX_ENTRY)
		acpigen_soc_clear_tx_gpio(SLP_S0_FP_EN);
	else if (entry == S0IX_EXIT)
		acpigen_soc_set_tx_gpio(SLP_S0_FP_EN);
}
