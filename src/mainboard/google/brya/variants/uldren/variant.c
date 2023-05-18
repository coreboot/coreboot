/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <sar.h>

static const struct pad_config touchscreen_disable_pads[] = {
	/* C0  : SMBCLK ==> EN_PP3300_TCHSCR_X */
	PAD_NC(GPP_C0, NONE),
	/* C1  : SMBDATA ==> TCHSCR_RST_L */
	PAD_NC(GPP_C1, NONE),
	/* H6  : I2C1_SDA ==> SOC_I2C_TCHSCR_SDA */
	PAD_NC(GPP_H6, NONE),
	/* H7  : I2C1_SCL ==> SOC_I2C_TCHSCR_SCL */
	PAD_NC(GPP_H7, NONE),
};

const char *get_wifi_sar_cbfs_filename(void)
{
	return "wifi_sar_0.hex";
}

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	if (fw_config_probe(FW_CONFIG(TOUCHSCREEN, TOUCHSCREEN_NONE))) {
		printk(BIOS_INFO, "Disable touchscreen GPIO pins.\n");
		gpio_padbased_override(padbased_table, touchscreen_disable_pads,
						ARRAY_SIZE(touchscreen_disable_pads));
	}
}
