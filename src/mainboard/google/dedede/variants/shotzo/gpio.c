/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>

/* Pad configuration in ramstage */
static const struct pad_config gpio_table[] = {
	/* A10 : LAN_PWR_ON */
	PAD_CFG_GPO(GPP_A10, 1, DEEP),
	/* A11 : TOUCH_RPT_EN */
	PAD_NC(GPP_A11, NONE),
	/* A12 : USB_OC1_N */
	PAD_CFG_NF(GPP_A12, NONE, DEEP, NF1),
	/* A13 : USB_OC2_N */
	PAD_CFG_NF(GPP_A13, NONE, DEEP, NF1),
	/* A18 : USB_OC0_N */
	PAD_CFG_NF(GPP_A18, NONE, DEEP, NF1),

	/* B3  : TRACKPAD_INT_ODL */
	PAD_NC(GPP_B3, NONE),

	/* C16 : I2C0_SDA */
	PAD_NC(GPP_C16, NONE),
	/* C17 : I2C0_SCL */
	PAD_NC(GPP_C17, NONE),
	/* C18 : LVDS_SDA */
	PAD_CFG_NF(GPP_C18, NONE, DEEP, NF1),
	/* C19 : LVDS_SCL */
	PAD_CFG_NF(GPP_C19, NONE, DEEP, NF1),

	/* D4 : TOUCH_INT_ODL */
	PAD_CFG_GPI_IRQ_WAKE(GPP_D4, NONE, PLTRST, LEVEL, INVERT),
	/* D12 : WCAM_RST_L */
	PAD_NC(GPP_D12, NONE),
	/* D15 : PD_EN_WEBCAM */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_D15, NONE, DEEP),

	/* E0  : CLK_24M_UCAM */
	PAD_NC(GPP_E0, NONE),
	/* E2  : CLK_24M_WCAM */
	PAD_NC(GPP_E2, NONE),

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

	/* H1  : EN_PP3300_SD_U */
	PAD_NC(GPP_H1, NONE),
	/* H6  : I2C3_SDA */
	PAD_NC(GPP_H6, NONE),
	/* H7  : I2C3_SCL */
	PAD_NC(GPP_H7, NONE),
};

const struct pad_config *variant_override_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}
