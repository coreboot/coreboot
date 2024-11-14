/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <fw_config.h>
#include <sar.h>
#include <acpi/acpigen.h>
#include <gpio.h>
#include <acpi/acpi.h>
#include <baseboard/variants.h>


const char *get_wifi_sar_cbfs_filename(void)
{
	return get_wifi_sar_fw_config_filename(FW_CONFIG_FIELD(WIFI_SAR_ID));
}

void variant_generate_s0ix_hook(enum s0ix_entry entry)
{
	/* GPP_C3 control the 3.3V of user facing camera,
	pull down when do suspend and pull high when resume. */
	if (entry == S0IX_ENTRY)
		acpigen_soc_clear_tx_gpio(GPP_C3);
	else if (entry == S0IX_EXIT)
		acpigen_soc_set_tx_gpio(GPP_C3);
}
