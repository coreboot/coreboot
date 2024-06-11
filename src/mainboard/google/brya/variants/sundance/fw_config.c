/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <fw_config.h>

static const struct pad_config wwan_disable_pads[] = {
	/* A8  : WWAN_RF_DISABLE_ODL */
	PAD_NC(GPP_A8, NONE),
	/* A12 : WWAN_PCIE_WAKE_ODL */
	PAD_NC(GPP_A12, NONE),
	/* D5  : SRCCLKREQ0# ==> WWAN_CLKREQ_ODL */
	PAD_NC(GPP_D5, NONE),
	/* D6  : WWAN_EN */
	PAD_NC(GPP_D6, NONE),
	/* D15  : EN_PP2800_WCAM_X ==> WWAN_SAR_DETECT_2_ODL */
	PAD_NC(GPP_D15, NONE),
	/* F12 : WWAN_RST_L */
	PAD_NC_LOCK(GPP_F12, NONE, LOCK_CONFIG),
	/* H19 : SOC_I2C_SUB_INT_ODL */
	PAD_NC(GPP_H19, NONE),
	/* H21  : WCAM_MCLK_R ==> WWAN_PERST_L */
	PAD_NC_LOCK(GPP_H21, NONE, LOCK_CONFIG),
	/* H23 : WWAN_SAR_DETECT_ODL */
	PAD_NC(GPP_H23, NONE),
};

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	if (fw_config_probe(FW_CONFIG(WWAN, LTE_ABSENT))) {
		printk(BIOS_INFO, "Disable WWAN-related GPIO pins.\n");
		gpio_padbased_override(padbased_table, wwan_disable_pads,
						ARRAY_SIZE(wwan_disable_pads));
	}
}
