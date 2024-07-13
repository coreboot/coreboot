/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>

/* Pad configuration in ramstage */
static const struct pad_config gpio_table[] = {
	/* C18 : AP_I2C_EMR_SDA */
	PAD_NC(GPP_C18, NONE),
	/* C19 : AP_I2C_EMR_SCL */
	PAD_NC(GPP_C19, NONE),

	/* D12 : WCAM_RST_L */
	PAD_NC(GPP_D12, NONE),
	/* D13 : EN_PP2800_CAMERA */
	PAD_NC(GPP_D13, NONE),
	/* D14 : EN_PP1200_CAMERA */
	PAD_NC(GPP_D14, NONE),
	/* D19 : WWAN_WLAN_COEX1 */
	PAD_NC(GPP_D19, NONE),
	/* D20 : WWAN_WLAN_COEX2 */
	PAD_NC(GPP_D20, NONE),
	/* D21 : WWAN_WLAN_COEX3 */
	PAD_NC(GPP_D21, NONE),

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
