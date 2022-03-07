/* SPDX-License-Identifier: GPL-2.0-only */

#include <chip.h>
#include <fw_config.h>
#include <baseboard/variants.h>

void variant_update_soc_chip_config(struct soc_intel_alderlake_config *config)
{
	config->cnvi_bt_audio_offload = fw_config_probe(FW_CONFIG(AUDIO,
							MAX98373_NAU88L25B_I2S));
}
