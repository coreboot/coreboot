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

static const struct pad_config lte_disable_pads[] = {
	/* A8  : WWAN_RF_DISABLE_ODL */
	PAD_NC(GPP_A8, NONE),
	/* D6  : SRCCLKREQ1# ==> WWAN_EN */
	PAD_NC(GPP_D6, NONE),
	/* F12 : WWAN_RST_L */
	PAD_NC_LOCK(GPP_F12, NONE, LOCK_CONFIG),
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
	if (fw_config_probe(FW_CONFIG(DB_LTE, LTE_ABSENT))) {
		printk(BIOS_INFO, "Disable LTE GPIO pins.\n");
		gpio_padbased_override(padbased_table, lte_disable_pads,
						ARRAY_SIZE(lte_disable_pads));
	}
}
