/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <baseboard/variants.h>
#include <chip.h>
#include <fw_config.h>
#include <sar.h>

void variant_update_soc_chip_config(struct soc_intel_alderlake_config *config)
{
}

const char *get_wifi_sar_cbfs_filename(void)
{
	return get_wifi_sar_fw_config_filename(FW_CONFIG_FIELD(WIFI));
}
