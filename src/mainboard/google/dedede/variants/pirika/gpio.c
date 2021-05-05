/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>

/* Pad configuration in ramstage*/
static const struct pad_config gpio_table[] = {

	/* A11 : TOUCH_RPT_EN */
	PAD_NC(GPP_A11, NONE),

	/* C18 : AP_I2C_EMR_SDA */
	PAD_NC(GPP_C18, NONE),
	/* C19 : AP_I2C_EMR_SCL */
	PAD_NC(GPP_C19, NONE),

	/* D12 : WCAM_RST_L */
	PAD_NC(GPP_D12, NONE),
	/* D13 : EN_PP3300_CAMERA */
	PAD_CFG_GPO(GPP_D13, 1, PLTRST),
	/* D14 : EN_PP1200_CAMERA */
	PAD_NC(GPP_D14, NONE),
	/* D15 : UCAM_RST_L */
	PAD_NC(GPP_D15, NONE),
	/* D19 : WWAN_WLAN_COEX1 */
	PAD_NC(GPP_D19, NONE),
	/* D20 : WWAN_WLAN_COEX2 */
	PAD_NC(GPP_D20, NONE),
	/* D21 : WWAN_WLAN_COEX3 */
	PAD_NC(GPP_D21, NONE),

	/* E2  : CLK_24M_WCAM */
	PAD_NC(GPP_E2, NONE),
	/* E17 : HDMI_DDC_SCL */
	PAD_NC(GPP_E17, NONE),

	/* H6  : AP_I2C_CAM_SDA */
	PAD_NC(GPP_H6, NONE),
	/* H7  : AP_I2C_CAM_SCL */
	PAD_NC(GPP_H7, NONE),

	/* S2  : DMIC1_CLK */
	PAD_NC(GPP_S2, NONE),
	/* S3  : DMIC1_DATA */
	PAD_NC(GPP_S3, NONE),
};

const struct pad_config *variant_override_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}
