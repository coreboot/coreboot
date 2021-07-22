/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <commonlib/helpers.h>

/* Pad configuration in ramstage*/
static const struct pad_config gpio_table[] = {
	/* Community 0 - GpioGroup GPP_B */
	PAD_CFG_NF(GPP_B2, NONE, PLTRST, NF1),		/* PMC_VRALERT_N */
	PAD_CFG_NF(GPP_B3, NONE, PLTRST, NF4),		/* ESPI_ALERT0_N */
	PAD_CFG_NF(GPP_B4, NONE, PLTRST, NF4),		/* ESPI_ALERT1_N */
	PAD_NC(GPP_B9, NONE),				/* Not connected */
	PAD_NC(GPP_B10, NONE),				/* Not connected */
	PAD_CFG_NF(GPP_B11, NONE, PLTRST, NF1),		/* PMC_ALERT_N */
	PAD_NC(GPP_B14, NONE),				/* Not connected */
	PAD_CFG_NF(GPP_B15, NONE, PLTRST, NF5),		/* ESPI_CS1_N */
	PAD_NC(GPP_B18, NONE),				/* Not connected */
	PAD_NC(GPP_B19, NONE),				/* Not connected */
	PAD_NC(GPP_B20, NONE),				/* Not connected */
	PAD_NC(GPP_B21, NONE),				/* Not connected */
	PAD_NC(GPP_B22, NONE),				/* Not connected */
	PAD_NC(GPP_B23, NONE),				/* Not connected */

	/* Community 0 - GpioGroup GPP_T */
	PAD_CFG_NF(GPP_T4, UP_20K, DEEP, NF1),		/* PSE_GBE0_INT */
	PAD_CFG_NF(GPP_T5, DN_20K, DEEP, NF1),		/* PSE_GBE0_RST_N */
	PAD_CFG_NF(GPP_T6, NONE, DEEP, NF1),		/* PSE_GBE0_AUXTS */
	PAD_CFG_NF(GPP_T7, NONE, DEEP, NF1),		/* PSE_GBE0_PPS */
	PAD_CFG_NF(GPP_T12, NONE, DEEP, NF2),		/* SIO_UART0_RXD */
	PAD_CFG_NF(GPP_T13, NONE, DEEP, NF2),		/* SIO_UART0_TXD */

	/* Community 0 - GpioGroup GPP_G */
	PAD_NC(GPP_G8, NONE),				/* Not connected */
	PAD_NC(GPP_G9, NONE),				/* Not connected */
	PAD_NC(GPP_G12, NONE),				/* Not connected */
	PAD_CFG_NF(GPP_G15, NONE, DEEP, NF1),		/* ESPI_IO_0 */
	PAD_CFG_NF(GPP_G16, NONE, DEEP, NF1),		/* ESPI_IO_1 */
	PAD_CFG_NF(GPP_G17, NONE, DEEP, NF1),		/* ESPI_IO_2 */
	PAD_CFG_NF(GPP_G18, NONE, DEEP, NF1),		/* ESPI_IO_3 */
	PAD_CFG_GPI(GPP_G19, UP_20K, PLTRST),		/* TPM_IRQ_N */
	PAD_CFG_NF(GPP_G20, NONE, DEEP, NF1),		/* ESPI_CSO_N */
	PAD_CFG_NF(GPP_G21, NONE, DEEP, NF1),		/* ESPI_CLK */
	PAD_CFG_NF(GPP_G22, NONE, DEEP, NF1),		/* ESPI_RST0_N */

	/* Community 1 - GpioGroup GPP_V */
	PAD_CFG_NF(GPP_V0, UP_20K, DEEP, NF1),		/* EMMC_CMD */
	PAD_CFG_NF(GPP_V1, UP_20K, DEEP, NF1),		/* EMMC_DATA0 */
	PAD_CFG_NF(GPP_V2, UP_20K, DEEP, NF1),		/* EMMC_DATA1 */
	PAD_CFG_NF(GPP_V3, UP_20K, DEEP, NF1),		/* EMMC_DATA2 */
	PAD_CFG_NF(GPP_V4, UP_20K, DEEP, NF1),		/* EMMC_DATA3 */
	PAD_CFG_NF(GPP_V5, UP_20K, DEEP, NF1),		/* EMMC_DATA4 */
	PAD_CFG_NF(GPP_V6, UP_20K, DEEP, NF1),		/* EMMC_DATA5 */
	PAD_CFG_NF(GPP_V7, UP_20K, DEEP, NF1),		/* EMMC_DATA6 */
	PAD_CFG_NF(GPP_V8, UP_20K, DEEP, NF1),		/* EMMC_DATA7 */
	PAD_CFG_NF(GPP_V9, DN_20K, DEEP, NF1),		/* EMMC_RCLK */
	PAD_CFG_NF(GPP_V10, DN_20K, DEEP, NF1),		/* EMMC_CLK */
	PAD_CFG_NF(GPP_V11, NONE, DEEP, NF1),		/* EMMC_RESET_N */

	/* Community 1 - GpioGroup GPP_H */
	PAD_CFG_NF(GPP_H0, DN_20K, DEEP, NF1),		/* PSE_GBE1_INT */
	PAD_CFG_NF(GPP_H1, DN_20K, DEEP, NF1),		/* PSE_GBE1_RST_N */
	PAD_CFG_NF(GPP_H2, NONE, DEEP, NF1),		/* PSE_GBE1_AUXTS */
	PAD_CFG_NF(GPP_H3, NONE, DEEP, NF1),		/* PSE_GBE1_PPS */
	PAD_CFG_NF(GPP_H8, UP_20K, DEEP, NF1),		/* SIO_I2C4_SDA */
	PAD_CFG_NF(GPP_H9, UP_20K, DEEP, NF1),		/* SIO_I2C4_SCL */

	/* Community 1 - GpioGroup GPP_D */
	PAD_CFG_GPO(GPP_D16, 0, DEEP),			/* EMMC_PWR_EN_N */

	/* Community 1 - GpioGroup GPP_U */
	PAD_CFG_NF(GPP_U0, DN_20K, DEEP, NF1),		/* GBE_INT */
	PAD_CFG_NF(GPP_U1, DN_20K, DEEP, NF1),		/* GBE_RST_N */
	PAD_CFG_NF(GPP_U2, NONE, DEEP, NF1),		/* GBE_PPS */
	PAD_CFG_NF(GPP_U3, NONE, DEEP, NF1),		/* GBE_AUXTS */
	PAD_NC(GPP_U12, NONE),				/* Not connected */
	PAD_NC(GPP_U13, NONE),				/* Not connected */
	PAD_NC(GPP_U16, NONE),				/* Not connected */
	PAD_NC(GPP_U17, NONE),				/* Not connected */
	PAD_NC(GPP_U18, NONE),				/* Not connected */
	PAD_CFG_GPO(GPP_U19, 1, DEEP),			/* UPD_REQ_N */

	/* Community 2 - GpioGroup DSW */
	PAD_CFG_NF(GPD4, NONE, PLTRST, NF1),		/* SLP_S3 */
	PAD_CFG_NF(GPD5, NONE, PLTRST, NF1),		/* SLP_S4 */
	PAD_NC(GPD7, NONE),				/* Not connected */
	PAD_CFG_NF(GPD10, NONE, PLTRST, NF1),		/* SLP_S5 */

	/* Community 3 - GpioGroup GPP_S */
	PAD_NC(GPP_S0, NONE),				/* Not connected */
	PAD_NC(GPP_S1, NONE),				/* Not connected */

	/* Community 3 - GpioGroup GPP_A */
	PAD_CFG_NF(GPP_A0, NONE, DEEP, NF1),		/* PSE_GBE0_RGMII_TXD3 */
	PAD_CFG_NF(GPP_A1, NONE, DEEP, NF1),		/* PSE_GBE0_RGMII_TXD2 */
	PAD_CFG_NF(GPP_A2, NONE, DEEP, NF1),		/* PSE_GBE0_RGMII_TXD1 */
	PAD_CFG_NF(GPP_A3, NONE, DEEP, NF1),		/* PSE_GBE0_RGMII_TXD0 */
	PAD_CFG_NF(GPP_A4, NONE, DEEP, NF1),		/* PSE_GBE0_RGMII_TXCLK */
	PAD_CFG_NF(GPP_A5, NONE, DEEP, NF1),		/* PSE_GBE0_RGMII_TXCTL */
	PAD_CFG_NF(GPP_A6, NONE, DEEP, NF1),		/* PSE_GBE0_RGMII_RXCLK */
	PAD_CFG_NF(GPP_A7, NONE, DEEP, NF1),		/* PSE_GBE0_RGMII_RXD3 */
	PAD_CFG_NF(GPP_A8, NONE, DEEP, NF1),		/* PSE_GBE0_RGMII_RXD2 */
	PAD_CFG_NF(GPP_A9, NONE, DEEP, NF1),		/* PSE_GBE0_RGMII_RXD1 */
	PAD_CFG_NF(GPP_A10, NONE, DEEP, NF1),		/* PSE_GBE0_RGMII_RXD0 */
	PAD_CFG_NF(GPP_A23, NONE, DEEP, NF1),		/* PSE_GBE0_RGMII_RXCTL */

	/* Community 4 - GpioGroup GPP_C */
	PAD_CFG_NF(GPP_C3, NONE, DEEP, NF1),		/* PSE_GBE0_MDC */
	PAD_CFG_NF(GPP_C4, NONE, DEEP, NF1),		/* PSE_GBE0_MDIO */
	PAD_NC(GPP_C5, NONE),				/* Not connected */
	PAD_CFG_NF(GPP_C6, NONE, DEEP, NF1),		/* PSE_GBE0_AUXTS */
	PAD_CFG_NF(GPP_C7, NONE, DEEP, NF1),		/* PSE_GBE0_PPS */
	PAD_NC(GPP_C8, NONE),				/* Not connected */
	PAD_CFG_NF(GPP_C12, NONE, DEEP, NF4),		/* SIO_UART1_RXD */
	PAD_CFG_NF(GPP_C13, NONE, DEEP, NF4),		/* SIO_UART1_TXD */
	PAD_CFG_NF(GPP_C16, NONE, DEEP, NF1),		/* GBE_MDIO */
	PAD_CFG_NF(GPP_C17, NONE, DEEP, NF1),		/* GBE_MDC */
	PAD_CFG_NF(GPP_C18, NONE, DEEP, NF4),		/* SIO_I2C1_SDA */
	PAD_CFG_NF(GPP_C19, NONE, DEEP, NF4),		/* SIO_I2C1_SCL */

	/* Community 4 - GpioGroup GPP_F */
	PAD_NC(GPP_F0, NONE),				/* Not connected */
	PAD_NC(GPP_F1, NONE),				/* Not connected */
	PAD_NC(GPP_F2, NONE),				/* Not connected */
	PAD_NC(GPP_F3, NONE),				/* Not connected */
	PAD_NC(GPP_F4, NONE),				/* Not connected */
	PAD_NC(GPP_F5, NONE),				/* Not connected */
	PAD_NC(GPP_F7, NONE),				/* Not connected */
	PAD_NC(GPP_F8, NONE),				/* Not connected */
	PAD_NC(GPP_F10, NONE),				/* Not connected */
	PAD_NC(GPP_F11, NONE),				/* Not connected */
	PAD_NC(GPP_F12, NONE),				/* Not connected */
	PAD_NC(GPP_F13, NONE),				/* Not connected */
	PAD_NC(GPP_F14, NONE),				/* Not connected */
	PAD_NC(GPP_F15, NONE),				/* Not connected */
	PAD_NC(GPP_F16, NONE),				/* Not connected */
	PAD_NC(GPP_F17, NONE),				/* Not connected */
	PAD_NC(GPP_F20, NONE),				/* Not connected */
	PAD_NC(GPP_F21, NONE),				/* Not connected */

	/* Community 4 - GpioGroup GPP_E */
	PAD_CFG_NF(GPP_E0, NONE, DEEP, NF1),		/* SATA_LED_N */
	PAD_CFG_NF(GPP_E3, NONE, DEEP, NF1),		/* DDI1_HPD */
	PAD_CFG_NF(GPP_E5, NONE, DEEP, NF1),		/* DDI1_DDC_SDA */
	PAD_NC(GPP_E6, NONE),				/* Not connected */
	PAD_CFG_NF(GPP_E7, NONE, DEEP, NF1),		/* DDI1_DDC_SCL */
	PAD_CFG_NF(GPP_E14, NONE, DEEP, NF1),		/* DDI0_HPD */
	PAD_NC(GPP_E15, NONE),				/* Not connected */
	PAD_NC(GPP_E16, NONE),				/* Not connected */
	PAD_CFG_NF(GPP_E18, NONE, DEEP, NF1),		/* DDI0_DDC_SDA */
	PAD_CFG_NF(GPP_E19, NONE, DEEP, NF1),		/* DDI0_DDC_SCL */
	PAD_NC(GPP_E23, NONE),				/* Not connected */

	/* Community 5 - GpioGroup GPP_R */
	PAD_NC(GPP_R1, NONE),				/* Not connected */
	PAD_NC(GPP_R2, NONE),				/* Not connected */
	PAD_NC(GPP_R3, NONE),				/* Not connected */
};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	PAD_CFG_NF(GPP_C0, NONE, DEEP, NF1),		/* SMB_CLK */
	PAD_CFG_NF(GPP_C1, NONE, DEEP, NF1),		/* SMB_DATA */
	PAD_CFG_NF(GPP_C2, NONE, DEEP, NF2),		/* SMB_ALERT_N */
	PAD_CFG_NF(GPP_C20, NONE, DEEP, NF4),		/* SIO_UART2_RXD */
	PAD_CFG_NF(GPP_C21, NONE, DEEP, NF4),		/* SIO_UART2_TXD */
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
