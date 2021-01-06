/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>

/* Pad configuration in ramstage*/
static const struct pad_config gpio_table[] = {
	/* A11 : TOUCH_RPT_EN ==> NC */
	PAD_NC(GPP_A11, NONE),

	/* B8  : WLAN_CLKREQ_ODL ==> NC */
	PAD_NC(GPP_B8, NONE),

	/* C18 : AP_I2C_EMR_SDA ==> NC */
	PAD_NC(GPP_C18, NONE),
	/* C19 : AP_I2C_EMR_SCL ==> NC */
	PAD_NC(GPP_C19, NONE),

	/* D1  : WLAN_PERST_L ==> NC */
	PAD_NC(GPP_D1, NONE),
	/* D3  : WLAN_PCIE_WAKE_ODL ==> NC */
	PAD_NC(GPP_D3, NONE),
	/* D4  : TOUCH_INT_ODL ==> NC */
	PAD_NC(GPP_D4, NONE),
	/* D5  : TOUCH_RESET_L ==> NC */
	PAD_NC(GPP_D5, NONE),
	/* D6  : EN_PP3300_TOUCH_S0 ==> NC */
	PAD_NC(GPP_D6, NONE),
	/* D12 : WCAM_RST_L ==> NC */
	PAD_NC(GPP_D12, NONE),
	/* D14 : EN_PP1200_CAMERA ==> NC */
	PAD_NC(GPP_D14, NONE),
	/* D15 : UCAM_RST_L ==> NC */
	PAD_NC(GPP_D15, NONE),
	/* D19 : WWAN_WLAN_COEX1 ==> NC */
	PAD_NC(GPP_D19, NONE),
	/* D20 : WWAN_WLAN_COEX2 ==> NC */
	PAD_NC(GPP_D20, NONE),
	/* D21 : WWAN_WLAN_COEX3 ==> NC */
	PAD_NC(GPP_D21, NONE),

	/* E2  : CLK_24M_WCAM ==> NC */
	PAD_NC(GPP_E2, NONE),

	/* G7  : SD_SDIO_WP ==> NC */
	PAD_NC(GPP_G7, NONE),

	/* H4  : AP_I2C_TS_SDA ==> NC */
	PAD_NC(GPP_H4, NONE),
	/* H5  : AP_I2C_TS_SCL ==> NC */
	PAD_NC(GPP_H5, NONE),
	/* H6  : AP_I2C_CAM_SDA ==> NC */
	PAD_NC(GPP_H6, NONE),
	/* H7  : AP_I2C_CAM_SCL ==> NC */
	PAD_NC(GPP_H7, NONE),

	/* S2  : DMIC1_CLK ==> NC */
	PAD_NC(GPP_S2, NONE),
	/* S3  : DMIC1_DATA ==> NC */
	PAD_NC(GPP_S3, NONE),
};

const struct pad_config *variant_override_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}
