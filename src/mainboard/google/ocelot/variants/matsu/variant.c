/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <ec/google/chromeec/ec.h>
#include <fsp/api.h>
#include <fw_config.h>
#include <sar.h>
#include <soc/gpio_soc_defs.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	return get_wifi_sar_fw_config_filename(FW_CONFIG_FIELD(WIFI_INTERFACE));
}
