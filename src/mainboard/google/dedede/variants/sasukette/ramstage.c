/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <device/device.h>
#include <device/path.h>
#include <fw_config.h>
#include <soc/pci_devs.h>
#include <drivers/i2c/generic/chip.h>
#include <soc/soc_chip.h>

extern struct chip_operations drivers_i2c_generic_ops;

static void audio_codec_update(void)
{
	const struct device_path codec_path[] = {
		{.type = DEVICE_PATH_PCI, .pci.devfn = PCH_DEVFN_I2C4},
		{.type = DEVICE_PATH_I2C, .i2c.device = 0x1a}
	};
	const struct device *codec =
		find_dev_nested_path(pci_root_bus(), codec_path, ARRAY_SIZE(codec_path));
	struct drivers_i2c_generic_config *config;

	if (!codec || (codec->chip_ops != &drivers_i2c_generic_ops) || !codec->chip_info)
		return;

	config = codec->chip_info;
	if (fw_config_probe(FW_CONFIG(AUDIO_CODEC_SOURCE, AUDIO_CODEC_ALC5682)))
		config->hid = "10EC5682";
	else if (fw_config_probe(FW_CONFIG(AUDIO_CODEC_SOURCE, AUDIO_CODEC_ALC5682I_VS)))
		config->hid = "RTL5682";
}

static void ext_vr_update(void)
{
	struct soc_intel_jasperlake_config *cfg = config_of_soc();

	if (fw_config_probe(FW_CONFIG(EXT_VR, EXT_VR_ABSENT)))
		cfg->disable_external_bypass_vr = 1;
}

void variant_devtree_update(void)
{
	audio_codec_update();
	ext_vr_update();
}
