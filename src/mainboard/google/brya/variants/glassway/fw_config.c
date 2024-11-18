/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <fw_config.h>

static const struct pad_config hdmi_disable_pads[] = {
	/* A20  : DDSP_HPD2 ==> NC */
	PAD_NC_LOCK(GPP_A20, NONE, LOCK_CONFIG),
	/* E20 : DDP2_CTRLCLK ==> NC */
	PAD_NC_LOCK(GPP_E20, NONE, LOCK_CONFIG),
	/* E21 : DDP2_CTRLDATA ==> NC */
	PAD_NC_LOCK(GPP_E21, NONE, LOCK_CONFIG),
};

static const struct pad_config lte_disable_pads[] = {
	/* A8  : WWAN_RF_DISABLE_ODL */
	PAD_NC(GPP_A8, NONE),
	/* D6  : WWAN_EN */
	PAD_NC(GPP_D6, NONE),
	/* F12 : WWAN_RST_L */
	PAD_NC_LOCK(GPP_F12, NONE, LOCK_CONFIG),
	/* H19 : SOC_I2C_SUB_INT_ODL */
	PAD_NC(GPP_H19, NONE),
	/* H23 : WWAN_SAR_DETECT_ODL */
	PAD_NC(GPP_H23, NONE),
};

static const struct pad_config stylus_disable_pads[] = {
	/* F13 : SOC_PEN_DETECT_R_ODL */
	PAD_NC_LOCK(GPP_F13, NONE, LOCK_CONFIG),
	/* F15 : SOC_PEN_DETECT_ODL */
	PAD_NC_LOCK(GPP_F15, NONE, LOCK_CONFIG),
};

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	if (!fw_config_probe(FW_CONFIG(DB_USB, DB_HDMI_LTE))) {
		printk(BIOS_INFO, "Disable HDMI GPIO pins.\n");
		gpio_padbased_override(padbased_table, hdmi_disable_pads,
						ARRAY_SIZE(hdmi_disable_pads));
	}

	/* Set to Disable LTE-related GPIO pins when field DB_USB is not DB_1C_LTE and DB_HDMI_LTE. */
	if (!fw_config_probe(FW_CONFIG(DB_USB, DB_1C_LTE))
		&& !fw_config_probe(FW_CONFIG(DB_USB, DB_HDMI_LTE))) {
		printk(BIOS_INFO, "Disable LTE GPIO pins.\n");
		gpio_padbased_override(padbased_table, lte_disable_pads,
						ARRAY_SIZE(lte_disable_pads));
	}

	if (fw_config_probe(FW_CONFIG(STYLUS, STYLUS_ABSENT))) {
		printk(BIOS_INFO, "Disable Stylus GPIO pins.\n");
		gpio_padbased_override(padbased_table, stylus_disable_pads,
						ARRAY_SIZE(stylus_disable_pads));
	}
}
