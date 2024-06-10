/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <baseboard/variants.h>
#include <variant/gpio.h>
#include <chip.h>
#include <fw_config.h>
#include <sar.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	return "wifi_sar_0.hex";
}

void variant_generate_s0ix_hook(enum s0ix_entry entry)
{
	/* Add board-specific MS0X entries */
	if (entry == S0IX_ENTRY)
		acpigen_soc_clear_tx_gpio(CAM_PWR);
	if (entry == S0IX_EXIT)
		acpigen_soc_set_tx_gpio(CAM_PWR);
}
