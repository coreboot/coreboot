/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <baseboard/variants.h>
#include <fw_config.h>
#include <sar.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	return get_wifi_sar_fw_config_filename(FW_CONFIG_FIELD(WIFI));
}
