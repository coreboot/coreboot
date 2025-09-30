/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <fw_config.h>

static const struct pad_config ish_disable_pads[] = {
	/* B3  : ISH_IMU_INT_L */
	PAD_NC_LOCK(GPP_B3, NONE, LOCK_CONFIG),
	/* B4  : ISH_ACC_INT_L */
	PAD_NC_LOCK(GPP_B4, NONE, LOCK_CONFIG),
	/* B5  : EC_I2C_SENSOR_SDA_SOC */
	PAD_NC_LOCK(GPP_B5, NONE, LOCK_CONFIG),
	/* B6  : EC_I2C_SENSOR_SCL_SOC */
	PAD_NC_LOCK(GPP_B6, NONE, LOCK_CONFIG),
};

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	if (fw_config_probe(FW_CONFIG(TABLET_MODE, TABLET_MODE_DISABLE))) {
		printk(BIOS_INFO, "Disable ISH-related GPIO pins.\n");
		gpio_padbased_override(padbased_table, ish_disable_pads,
						ARRAY_SIZE(ish_disable_pads));
	}
}
