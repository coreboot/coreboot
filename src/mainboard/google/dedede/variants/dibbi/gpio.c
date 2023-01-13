/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>

/* Pad configuration in ramstage */
static const struct pad_config gpio_table[] = {
	/* A11 : TOUCH_RPT_EN */
	PAD_NC(GPP_A11, NONE),
	/* A12 : USB_OC1_N */
	PAD_CFG_NF(GPP_A12, NONE, DEEP, NF1),
	/* A13 : USB_OC2_N */
	PAD_CFG_NF(GPP_A13, NONE, DEEP, NF1),
	/* A14 : USB_OC3_N */
	PAD_CFG_NF(GPP_A14, NONE, DEEP, NF1),
	/* A18 : USB_OC0_N */
	PAD_CFG_NF(GPP_A18, NONE, DEEP, NF1),

	/* D2  : PWM_PP3300_BUZZER */
	PAD_CFG_GPO(GPP_D2, 1, DEEP),
	/* D4  : LAN_PE_ISOLATE_ODL_R */
	PAD_CFG_GPO(GPP_D4, 1, DEEP),
	/* D5  : TOUCH_RESET_L */
	PAD_NC(GPP_D5, NONE),
	/* D6  : EN_PP3300_TOUCH_S0 */
	PAD_NC(GPP_D6, NONE),
	/* D17 : LAN_PERST_L */
	PAD_CFG_GPO(GPP_D17, 1, PLTRST),
	/* D19 : WWAN_WLAN_COEX1 */
	PAD_NC(GPP_D19, NONE),
	/* D20 : WWAN_WLAN_COEX2 */
	PAD_NC(GPP_D20, NONE),

	/* E13 : GPP_E13/DDI0_DDC_SCL */
	PAD_CFG_NF(GPP_E13, NONE, DEEP, NF1),
	/* E14 : GPP_E14/DDI0_DDC_SDA */
	PAD_CFG_NF(GPP_E14, NONE, DEEP, NF1),
	/* E15 : GPP_E15/DDI1_DDC_SCL */
	PAD_CFG_NF(GPP_E15, NONE, DEEP, NF1),
	/* E16 : GPP_E16/DDI1_DDC_SDA */
	PAD_CFG_NF(GPP_E16, NONE, DEEP, NF1),

	/* G0  : SD_CMD */
	PAD_NC(GPP_G0, NONE),
	/* G1  : SD_DATA0 */
	PAD_NC(GPP_G1, NONE),
	/* G2  : SD_DATA1 */
	PAD_NC(GPP_G2, NONE),
	/* G3  : SD_DATA2 */
	PAD_NC(GPP_G3, NONE),
	/* G4  : SD_DATA3 */
	PAD_NC(GPP_G4, NONE),
	/* G5  : SD_CD_ODL */
	PAD_NC(GPP_G5, NONE),
	/* G6  : SD_CLK */
	PAD_NC(GPP_G6, NONE),
	/* G7  : SD_SDIO_WP */
	PAD_NC(GPP_G7, NONE),

	/* H4  : AP_I2C_TS_SDA */
	PAD_NC(GPP_H4, NONE),
	/* H5  : AP_I2C_TS_SCL */
	PAD_NC(GPP_H5, NONE),
	/* H6  : AP_I2C_CAM_SDA */
	PAD_NC(GPP_H6, NONE),
	/* H7  : AP_I2C_CAM_SCL */
	PAD_NC(GPP_H7, NONE),
	/* H15 : I2S_SPK_BCLK */
	PAD_NC(GPP_H15, NONE),

	/* R6  : I2S_SPK_LRCK */
	PAD_NC(GPP_R6, NONE),
	/* R7  :  I2S_SPK_AUDIO */
	PAD_NC(GPP_R7, NONE),

	/* S2  : DMIC1_CLK */
	PAD_NC(GPP_S2, NONE),
	/* S3  : DMIC1_DATA */
	PAD_NC(GPP_S3, NONE),
	/* S6  :  DMIC0_CLK */
	PAD_NC(GPP_S6, NONE),
	/* S7  : DMIC0_DATA */
	PAD_NC(GPP_S7, NONE),
};

const struct pad_config *variant_override_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}
