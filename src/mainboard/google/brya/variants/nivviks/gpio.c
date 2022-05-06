/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <boardid.h>
#include <commonlib/helpers.h>
#include <soc/gpio.h>

/* Pad configuration in ramstage for nivviks board_id = 0 */
static const struct pad_config board_id0_overrides[] = {
	/* R4 : I2S2_SCLK ==> I2S_SPK_BCLK_R */
	PAD_CFG_NF(GPP_R4, NONE, DEEP, NF2),
	/* R5 : I2S2_SFRM ==> I2S_SPK_LRCK_R */
	PAD_CFG_NF(GPP_R5, NONE, DEEP, NF2),
	/* R6 : I2S2_TXD ==> I2S_SPK_AUDIO_R */
	PAD_CFG_NF(GPP_R6, NONE, DEEP, NF2),
	/* R7 : I2S2_RXD ==> NC */
	PAD_NC(GPP_R7, NONE),
	/* S0  : NC */
	PAD_NC(GPP_S0, NONE),
	/* S1  : NC */
	PAD_NC(GPP_S1, NONE),
	/* S2 : DMIC_CKL_A_0 ==> DMIC_UCAM_CLK_R */
	PAD_CFG_NF(GPP_S2, NONE, DEEP, NF2),
	/* S3 : DMIC_DATA_0 ==> DMIC_UCAM_DATA */
	PAD_CFG_NF(GPP_S3, NONE, DEEP, NF2),
	/* S6  : DMIC_CLK_A_1 ==> DMIC_WCAM_CLK_R */
	PAD_CFG_NF(GPP_S6, NONE, DEEP, NF2),
	/* S7  : DMIC_DATA_1 ==> DMIC_WCAM_DATA */
	PAD_CFG_NF(GPP_S7, NONE, DEEP, NF2),
};

/* Pad configuration in ramstage for nirwen */
static const struct pad_config override_gpio_table_nirwen[] = {
	/* B4  : SSD_PERST_L */
	PAD_CFG_GPO(GPP_B4, 1, DEEP),
	/* D6  : SRCCLKREQ1# ==> SSD_CLKREQ_ODL */
	PAD_CFG_NF(GPP_D6, NONE, DEEP, NF1),
	/* D11 : EN_PP3300_SSD */
	PAD_CFG_GPO(GPP_D11, 1, DEEP),
	/* E13  : SRCCLKREQ1# ==> WWAN_EN */
	PAD_CFG_GPO(GPP_E13, 1, DEEP),
	/* E17 : SSD_PLN_L */
	PAD_CFG_GPO(GPP_E17, 1, PLTRST),
};

/* Early pad configuration in bootblock for nivviks */
static const struct pad_config early_gpio_table[] = {
	/* F12 : GSXDOUT ==> WWAN_RST_L */
	PAD_CFG_GPO(GPP_F12, 0, DEEP),
	/* H12 : UART0_RTS# ==> SD_PERST_L */
	PAD_CFG_GPO(GPP_H12, 0, DEEP),
	/* A13 : GPP_A13 ==> GSC_SOC_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A13, NONE, PLTRST, LEVEL, INVERT),
	/* D6  : SRCCLKREQ1# ==> WWAN_EN */
	PAD_CFG_GPO(GPP_D6, 1, DEEP),
	/* E12 : THC0_SPI1_IO1 ==> SOC_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_E12, NONE, DEEP),
	/* F18 : THC1_SPI2_INT# ==> EC_IN_RW_OD */
	PAD_CFG_GPI(GPP_F18, NONE, DEEP),
	/* H4  : I2C0_SDA ==> SOC_I2C_GSC_SDA */
	PAD_CFG_NF(GPP_H4, NONE, DEEP, NF1),
	/* H5  : I2C0_SCL ==> SOC_I2C_GSC_SCL */
	PAD_CFG_NF(GPP_H5, NONE, DEEP, NF1),
	/* H10 : UART0_RXD ==> UART_SOC_RX_DBG_TX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* H11 : UART0_TXD ==> UART_SOC_TX_DBG_RX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),
	/* H13 : UART0_CTS# ==> EN_PP3300_SD_X */
	PAD_CFG_GPO(GPP_H13, 1, DEEP),
};

/* Early pad configuration in bootblock for nirwen */
static const struct pad_config early_gpio_table_nirwen[] = {
	/* B4  : SSD_PERST_L */
	PAD_CFG_GPO(GPP_B4, 0, DEEP),
	/* F12 : GSXDOUT ==> WWAN_RST_L */
	PAD_CFG_GPO(GPP_F12, 0, DEEP),
	/* H12 : UART0_RTS# ==> SD_PERST_L */
	PAD_CFG_GPO(GPP_H12, 0, DEEP),
	/* A13 : GPP_A13 ==> GSC_SOC_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A13, NONE, PLTRST, LEVEL, INVERT),
	/* E12 : THC0_SPI1_IO1 ==> SOC_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_E12, NONE, DEEP),
	/* E13  : SRCCLKREQ1# ==> WWAN_EN */
	PAD_CFG_GPO(GPP_E13, 1, DEEP),
	/* F18 : THC1_SPI2_INT# ==> EC_IN_RW_OD */
	PAD_CFG_GPI(GPP_F18, NONE, DEEP),
	/* H4  : I2C0_SDA ==> SOC_I2C_GSC_SDA */
	PAD_CFG_NF(GPP_H4, NONE, DEEP, NF1),
	/* H5  : I2C0_SCL ==> SOC_I2C_GSC_SCL */
	PAD_CFG_NF(GPP_H5, NONE, DEEP, NF1),
	/* H10 : UART0_RXD ==> UART_SOC_RX_DBG_TX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* H11 : UART0_TXD ==> UART_SOC_TX_DBG_RX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),
	/* D11 : EN_PP3300_SSD */
	PAD_CFG_GPO(GPP_D11, 1, DEEP),
	/* H13 : UART0_CTS# ==> EN_PP3300_SD_X */
	PAD_CFG_GPO(GPP_H13, 1, DEEP),
};

static const struct pad_config romstage_gpio_table[] = {
	/* B4  : SSD_PERST_L */
	PAD_CFG_GPO(GPP_B4, 1, DEEP),
	/* H12 : UART0_RTS# ==> SD_PERST_L */
	PAD_CFG_GPO(GPP_H12, 1, DEEP),
};

const struct pad_config *variant_gpio_override_table(size_t *num)
{
	const uint32_t id = board_id();
	if (id == BOARD_ID_UNKNOWN || id == 0) {
		*num = ARRAY_SIZE(board_id0_overrides);
		return board_id0_overrides;
	}

	if (id == 1) {
		*num = 0;
		return NULL;
	}

	/* board_id >= 2 means nirwen */
	*num = ARRAY_SIZE(override_gpio_table_nirwen);
	return override_gpio_table_nirwen;
}

const struct pad_config *variant_early_gpio_table(size_t *num)
{
	const uint32_t id = board_id();
	if (id == BOARD_ID_UNKNOWN || id < 2) {
		*num = ARRAY_SIZE(early_gpio_table);
		return early_gpio_table;
	}

	/* board_id >= 2 means nirwen */
	*num = ARRAY_SIZE(early_gpio_table_nirwen);
	return early_gpio_table_nirwen;
}


const struct pad_config *variant_romstage_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(romstage_gpio_table);
	return romstage_gpio_table;
}
