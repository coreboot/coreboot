/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>

/* Pad configuration in ramstage*/
static const struct pad_config gpio_table[] = {
	/* ToDo: Fill other gpio configuration */

	/* WWAN_WAKE_N */
	PAD_CFG_GPI_SCI(GPP_A10, NONE, DEEP, LEVEL, INVERT),

	/* M.2_WWAN_DISABLE_N */
	PAD_CFG_GPO(GPP_A19, 1, PLTRST),

	/* M.2_WLAN_PERST_N */
	PAD_CFG_GPO(GPP_B17, 1, PLTRST),

	/* WWAN_PERST_N */
	PAD_CFG_GPO(GPP_C0, 0, PLTRST),

	/* M2_WWAN_SSD_SKT2_CFG2 */
	PAD_CFG_GPI(GPP_C3, NONE, PLTRST),

	/* I2C0_SDA */
	PAD_CFG_NF(GPP_C16, UP_2K, DEEP, NF1),

	/* I2C0_SCL */
	PAD_CFG_NF(GPP_C17, UP_2K, DEEP, NF1),

	/* WIFI_RF_KILL_N */
	PAD_CFG_GPO(GPP_D0, 1, PLTRST),

	/* BT_RF_KILL_N */
	PAD_CFG_GPO(GPP_D1, 1, PLTRST),

	/* I2S_MCLK */
	PAD_CFG_NF(GPP_D18, NONE, DEEP, NF1),

	/* WWAN_FCP_OFF_N */
	PAD_CFG_GPO(GPP_E3, 1, PLTRST),

	/* WWAN EN GPIO */
	PAD_CFG_GPO(GPP_H7, 1, PLTRST),

	/* M.2_BT_I2S2_SCLK */
	PAD_CFG_GPI(GPP_H11, NONE, PLTRST),

	/* M.2_BT_I2S2_RXD */
	PAD_CFG_GPI(GPP_H14, NONE, PLTRST),

	/* I2S1_SCLK */
	PAD_CFG_NF(GPP_H15, NONE, DEEP, NF1),

	/* Audio Jack Detection */
	PAD_CFG_GPI_INT(GPP_H16, NONE, PLTRST, EDGE_BOTH),

	/* M2_CNVI_EN_N */
	PAD_CFG_GPO(GPP_H19, 0, PLTRST),

	/* I2S0_SCLK */
	PAD_CFG_NF(GPP_R0, NONE, DEEP, NF2),

	/* I2S0_SFRM */
	PAD_CFG_NF(GPP_R1, NONE, DEEP, NF2),

	/* I2S0_TXD */
	PAD_CFG_NF(GPP_R2, NONE, DEEP, NF2),

	/* I2S0_RXD */
	PAD_CFG_NF(GPP_R3, NONE, DEEP, NF2),

	/* I2S1_RXD */
	PAD_CFG_NF(GPP_R5, NONE, DEEP, NF2),

	/* I2S1_SFRM */
	PAD_CFG_NF(GPP_R6, NONE, DEEP, NF2),

	/* I2S1_TXD */
	PAD_CFG_NF(GPP_R7, NONE, DEEP, NF2),

	/* WWAN RST_N */
	PAD_CFG_GPO(GPP_S0, 1, DEEP),

	/* DMIC_CLK_1 */
	PAD_CFG_NF(GPP_S2, UP_20K, DEEP, NF2),

	/* DMIC_DATA_1 */
	PAD_CFG_NF(GPP_S3, UP_20K, DEEP, NF2),

	/* DMIC_CLK_0 */
	PAD_CFG_NF(GPP_S6, UP_20K, DEEP, NF2),

	/* DMIC_DATA_0 */
	PAD_CFG_NF(GPP_S7, UP_20K, DEEP, NF2),

};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* ToDo: Fill early gpio configurations for TPM */
};

const struct pad_config *variant_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}

const struct pad_config *variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AL(CROS_GPIO_VIRTUAL, CROS_GPIO_COMM0_NAME),
};

const struct cros_gpio *variant_cros_gpios(size_t *num)
{
	*num = ARRAY_SIZE(cros_gpios);
	return cros_gpios;
}
