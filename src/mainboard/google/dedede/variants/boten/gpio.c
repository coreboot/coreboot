/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <vendorcode/google/chromeos/chromeos.h>

/* Pad configuration in ramstage*/
static const struct pad_config gpio_table[] = {
	/* A10 : WWAN_EN => LTE_PWR_OFF_ODL */
	PAD_CFG_GPO(GPP_A10, 1, PWROK),

	/* B7  : WWAN_SAR_DETECT_R_ODL */
	PAD_CFG_GPO(GPP_B7, 1, DEEP),

	/* C12 : AP_PEN_DET_ODL */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_C12, UP_20K, DEEP),
	/* C18 : AP_I2C_EMR_SDA */
	PAD_NC(GPP_C18, NONE),
	/* C19 : AP_I2C_EMR_SCL */
	PAD_NC(GPP_C19, NONE),

	/* D12 : WCAM_RST_L */
	PAD_NC(GPP_D12, NONE),
	/* D13 : EN_PP2800_CAMERA */
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
	/* D22 : AP_I2C_SUB_SDA*/
	PAD_CFG_NF(GPP_D22, NONE, DEEP, NF1),
	/* D23 : AP_I2C_SUB_SCL */
	PAD_CFG_NF(GPP_D23, NONE, DEEP, NF1),

	/* E0  : CLK_24M_UCAM */
	PAD_NC(GPP_E0, NONE),
	/* E2  : CLK_24M_WCAM */
	PAD_NC(GPP_E2, NONE),
	/* E11 : AP_I2C_SUB_INT_ODL */
	PAD_CFG_GPI_INT(GPP_E11, NONE, PLTRST, EDGE_BOTH),

	/* H6  : AP_I2C_CAM_SDA */
	PAD_NC(GPP_H6, NONE),
	/* H7  : AP_I2C_CAM_SCL */
	PAD_NC(GPP_H7, NONE),
	/* H17 : WWAN_RST_L => LTE_RESET_R_ODL */
	PAD_CFG_GPO(GPP_H17, 0, PLTRST),
};

const struct pad_config *variant_override_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}
