/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <cpu/cpu.h>
#include <cpu/intel/cpu_ids.h>
#include <device/device.h>
#include <ec/ec.h>
#include <fw_config.h>
#include <smbios.h>
#include <soc/gpio.h>
#include <stdint.h>
#include <stdio.h>

#include "board_id.h"

const char *smbios_system_sku(void)
{
	static char sku_str[7] = "";
	uint8_t sku_id = get_board_id();

	snprintf(sku_str, sizeof(sku_str), "sku%u", sku_id);
	return sku_str;
}

static void mainboard_init(void *chip_info)
{
	variant_configure_gpio_pads();

	if (CONFIG(EC_GOOGLE_CHROMEEC))
		mainboard_ec_init();

	variant_devtree_update();
}

void __weak variant_devtree_update(void)
{
	/* Override dev tree settings per board */
}

#if CONFIG(BOARD_INTEL_ADLRVP_N_EXT_EC)
static void add_fw_config_oem_string(const struct fw_config *config, void *arg)
{
	struct smbios_type11 *t;
	char buffer[64];

	t = (struct smbios_type11 *)arg;

	snprintf(buffer, sizeof(buffer), "%s-%s", config->field_name, config->option_name);
	t->count = smbios_add_string(t->eos, buffer);
}

static void mainboard_smbios_strings(struct device *dev, struct smbios_type11 *t)
{
	fw_config_for_each_found(add_fw_config_oem_string, t);
}
#endif

static void mainboard_enable(struct device *dev)
{
#if CONFIG(BOARD_INTEL_ADLRVP_N_EXT_EC)
	dev->ops->get_smbios_strings = mainboard_smbios_strings;
#endif
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};
