/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <fw_config.h>
#include <drivers/i2c/hid/chip.h>
#include <device/device.h>
#include <static.h>

static void audio_codec_update(void)
{
	struct device *codec = DEV_PTR(audio_codec);
	struct drivers_i2c_generic_config *config;

	config = codec->chip_info;
	if (fw_config_probe(FW_CONFIG(AUDIO_CODEC_SOURCE, AUDIO_CODEC_ALC5682I_VS)))
		config->hid = "RTL5682";
	else
		config->hid = "10EC5682";
}
void variant_devtree_update(void)
{
	audio_codec_update();
}
