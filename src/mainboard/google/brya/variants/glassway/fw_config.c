/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <fw_config.h>

static const struct pad_config stylus_disable_pads[] = {
	/* F13 : SOC_PEN_DETECT_R_ODL */
	PAD_NC_LOCK(GPP_F13, NONE, LOCK_CONFIG),
	/* F15 : SOC_PEN_DETECT_ODL */
	PAD_NC_LOCK(GPP_F15, NONE, LOCK_CONFIG),
};

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	if (fw_config_probe(FW_CONFIG(STYLUS, STYLUS_ABSENT))) {
		printk(BIOS_INFO, "Disable Stylus GPIO pins.\n");
		gpio_padbased_override(padbased_table, stylus_disable_pads,
						ARRAY_SIZE(stylus_disable_pads));
	}
}
