/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>

/* Pad configuration in ramstage*/
static const struct pad_config gpio_table[] = {
	/* A10 : WWAN_EN */
	PAD_CFG_GPO(GPP_A10, 1, PWROK),
	/* A11 : TOUCH_RPT_EN ==> NC */
	PAD_NC(GPP_A11, NONE),

	/* B7  : PCIE_CLKREQ2_N ==> WWAN_SAR_DETECT_ODL*/
	PAD_CFG_GPO(GPP_B7, 1, DEEP),
	/* B8  : WLAN_CLKREQ_ODL ==> NC */
	PAD_NC(GPP_B8, NONE),

	/* D0  : WWAN_HOST_WAKE ==> WWAN_WDISABLE_L */
	PAD_CFG_GPO(GPP_D0, 1, DEEP),
	/* D1  : WLAN_PERST_L ==> NC */
	PAD_NC(GPP_D1, NONE),
	/* D3  : WLAN_PCIE_WAKE_ODL ==> NC */
	PAD_NC(GPP_D3, NONE),
	/* D5  : TOUCHSCREEN_RESET */
	PAD_CFG_GPO(GPP_D5, 1, DEEP),
	/* D7  : EMR_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_D7, NONE, PLTRST, LEVEL, INVERT),
	/* D13 : EN_PP3300_CAMERA */
	PAD_CFG_GPO(GPP_D13, 0, PLTRST),
	/* D15 : EN_PP2800_CAMERA */
	PAD_CFG_GPO(GPP_D15, 0, PLTRST),
	/* D19 : WWAN_WLAN_COEX1 ==> NC */
	PAD_NC(GPP_D19, NONE),
	/* D20 : WWAN_WLAN_COEX2 ==> NC */
	PAD_NC(GPP_D20, NONE),
	/* D21 : WWAN_WLAN_COEX3 ==> NC */
	PAD_NC(GPP_D21, NONE),
	/* D22 : AP_I2C_SUB_SDA*/
	PAD_CFG_NF(GPP_D22, NONE, DEEP, NF1),
	/* D23 : AP_I2C_SUB_SCL */
	PAD_CFG_NF(GPP_D23, NONE, DEEP, NF1),

	/* E0 : CLK_24M_UCAM ==> NC */
	PAD_NC(GPP_E0, NONE),
	/* E1 : EMR_RESET_L */
	PAD_CFG_GPO(GPP_E1, 0, DEEP),
	/* E13 : DDI0_DDC_SCL */
	PAD_CFG_NF(GPP_E13, NONE, DEEP, NF1),
	/* E14 : DDI0_DDC_SDA */
	PAD_CFG_NF(GPP_E14, NONE, DEEP, NF1),

	/* H17 : WWAN_RST_L */
	PAD_CFG_GPO(GPP_H17, 0, PLTRST),
};

const struct pad_config *variant_override_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}
