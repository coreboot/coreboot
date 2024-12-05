/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <boardid.h>
#include <console/console.h>
#include <fw_config.h>

static const struct pad_config lte_disable_pads_gothrax[] = {
	/* A8  : WWAN_RF_DISABLE_ODL */
	PAD_NC(GPP_A8, NONE),
	/* E13  : WWAN_EN */
	PAD_NC_LOCK(GPP_E13, NONE, LOCK_CONFIG),
	/* F12 : WWAN_RST_L */
	PAD_NC_LOCK(GPP_F12, NONE, LOCK_CONFIG),
	/* H19 : SOC_I2C_SUB_INT_ODL */
	PAD_NC(GPP_H19, NONE),
	/* H23 : WWAN_SAR_DETECT_ODL */
	PAD_NC(GPP_H23, NONE),
};

static const struct pad_config wfc_disable_pads[] = {
	/* R6 : DMIC_WCAM_CLK_R */
	PAD_NC(GPP_R6, NONE),
	/* R7 : DMIC_WCAM_DATA */
	PAD_NC(GPP_R7, NONE),
};

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	if (!(fw_config_probe(FW_CONFIG(DB_USB, DB_C_A_LTE)) ||
		fw_config_probe(FW_CONFIG(DB_USB, DB_A_HDMI_LTE)))) {
		printk(BIOS_INFO, "Disable LTE-related GPIO pins on gothrax.\n");
		gpio_padbased_override(padbased_table, lte_disable_pads_gothrax,
						ARRAY_SIZE(lte_disable_pads_gothrax));
	}

	if (!fw_config_probe(FW_CONFIG(WFC, WFC_PRESENT))) {
		printk(BIOS_INFO, "Disable WFC GPIO pins.\n");
		gpio_padbased_override(padbased_table, wfc_disable_pads,
						ARRAY_SIZE(wfc_disable_pads));
	}
}
