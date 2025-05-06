/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <drivers/intel/gma/opregion.h>
#include <soc/ramstage.h>
#include <option.h>
#include <variants.h>

enum cmos_power_profile get_power_profile(enum cmos_power_profile fallback)
{
	const unsigned int power_profile = get_uint_option("power_profile", fallback);
	return power_profile < NUM_POWER_PROFILES ? power_profile : fallback;
}

static void init_mainboard(void *chip_info)
{
	const struct pad_config *pads;
	size_t num;

	pads = variant_gpio_table(&num);
	gpio_configure_pads(pads, num);

	devtree_update();
}

struct chip_operations mainboard_ops = {
	.init = init_mainboard,
};

const char *mainboard_vbt_filename(void)
{
	if (CONFIG(BOARD_USES_FIXED_MODE_VBT) && get_uint_option("display_native_res", 0) == 1)
		return "vbt_native_res.bin";

	return "vbt.bin";
}
