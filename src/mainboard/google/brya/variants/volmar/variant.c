/* SPDX-License-Identifier: GPL-2.0-only */

#include <chip.h>
#include <fw_config.h>
#include <sar.h>
#include <baseboard/variants.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	return "wifi_sar_0.hex";
}

void variant_update_soc_chip_config(struct soc_intel_alderlake_config *config)
{
	config->cnvi_bt_audio_offload = fw_config_probe(FW_CONFIG(AUDIO,
							MAX98373_NAU88L25B_I2S));

	if (fw_config_probe(FW_CONFIG(FPMCU_MASK, FPMCU_DISABLED)))
		config->serial_io_gspi_mode[PchSerialIoIndexGSPI1] = PchSerialIoDisabled;

}
