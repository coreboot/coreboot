/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <boardid.h>
#include <device/device.h>
#include <drivers/i2c/hid/chip.h>

static void devtree_update_emmc_rtd3(uint32_t board_ver)
{
	struct device *emmc_rtd3 = DEV_PTR(emmc_rtd3);
	if (board_ver > 1)
		return;

	emmc_rtd3->enabled = 0;
}

static void devtree_update_audio_codec(uint32_t board_ver)
{
	struct device *audio_codec = DEV_PTR(audio_codec);
	struct drivers_i2c_generic_config *config = audio_codec->chip_info;

	if (board_ver <= 1)
		return;

	config->hid = "RTL5682";
	audio_codec->enabled = 1;
}

void variant_devtree_update(void)
{
	uint32_t board_ver = board_id();
	devtree_update_emmc_rtd3(board_ver);
	devtree_update_audio_codec(board_ver);
}
