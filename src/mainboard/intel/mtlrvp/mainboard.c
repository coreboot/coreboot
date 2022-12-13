/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>
#include <drivers/intel/gma/opregion.h>
#include <ec/ec.h>
#include <ec/intel/board_id.h>
#include <soc/ramstage.h>
#include <smbios.h>
#include <stdint.h>
#include <string.h>

const char *smbios_system_sku(void)
{
	static char sku_str[7] = "";
	uint8_t sku_id = get_rvp_board_id();

	snprintf(sku_str, sizeof(sku_str), "sku%u", sku_id);
	return sku_str;
}

const char *mainboard_vbt_filename(void)
{
	return "vbt.bin";
}

void mainboard_update_soc_chip_config(struct soc_intel_meteorlake_config *cfg)
{
	/* TODO: Update mainboard */
}

static void mainboard_init(void *chip_info)
{
	if (CONFIG(EC_GOOGLE_CHROMEEC))
		mainboard_ec_init();
}

static void mainboard_enable(struct device *dev)
{
	/* TODO: Enable mainboard */
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};
