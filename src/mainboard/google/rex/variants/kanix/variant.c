/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <chip.h>
#include <fw_config.h>
#include <sar.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	return get_wifi_sar_fw_config_filename(FW_CONFIG_FIELD(WIFI_TYPE));
}

void variant_update_soc_chip_config(struct soc_intel_meteorlake_config *config)
{
	config->cnvi_bt_audio_offload = true;
}
