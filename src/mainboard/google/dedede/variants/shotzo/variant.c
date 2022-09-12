/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <baseboard/variants.h>
#include <variant/gpio.h>

void variant_generate_s0ix_hook(enum s0ix_entry entry)
{
	if (entry == S0IX_ENTRY)
		acpigen_soc_clear_tx_gpio(GPP_A10);
	else if (entry == S0IX_EXIT)
		acpigen_soc_set_tx_gpio(GPP_A10);
}
