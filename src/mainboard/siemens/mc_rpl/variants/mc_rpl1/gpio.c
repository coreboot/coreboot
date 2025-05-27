/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <types.h>

/* Pad configuration in ramstage */
static const struct pad_config gpio_table[] = {
	/* GPIO A0-A6, A9-A10 default function is NF1 for eSPI interface when
	  eSPI is enabled */

	/* Community 0 - Gpio Group GPP_B */
	PAD_CFG_NF(GPP_B2, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_B14, NONE, DEEP, NF4),
	PAD_CFG_GPI(GPP_B23, NONE, PLTRST),

	/* Community 0 - Gpio Group GPP_A */
	PAD_CFG_NF(GPP_A0, NONE, DEEP, NF1),	/* eSPI */
	PAD_CFG_NF(GPP_A1, NONE, DEEP, NF1),	/* eSPI */
	PAD_CFG_NF(GPP_A2, NONE, DEEP, NF1),	/* eSPI */
	PAD_CFG_NF(GPP_A3, NONE, DEEP, NF1),	/* eSPI */
	PAD_CFG_NF(GPP_A4, NONE, DEEP, NF1),	/* eSPI */
	PAD_CFG_NF(GPP_A14, NONE, DEEP, NF1),	/* USB2 OC1 pin */
	PAD_CFG_NF(GPP_A15, NONE, DEEP, NF1),	/* USB2 OC2 pin */
	PAD_CFG_NF(GPP_A16, NONE, DEEP, NF1),	/* USB2 OC3 pin */
	PAD_CFG_NF(GPP_A23, NONE, DEEP, NF1),	/* eSPI */

	/* Community 1 - Gpio Group GPP_H */
	PAD_CFG_GPI(GPP_H0, NONE, PLTRST),
	PAD_CFG_GPI(GPP_H1, NONE, PLTRST),
	PAD_CFG_GPI(GPP_H2, NONE, PLTRST),
	PAD_CFG_NF(GPP_H3, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_H6, NONE, DEEP, NF1),	/* I2C1_SDA */
	PAD_CFG_NF(GPP_H7, NONE, DEEP, NF1),	/* I2C1_SCL */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),	/* UART0_RXD */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),	/* UART0_TXD */
	PAD_CFG_NF(GPP_H19, NONE, DEEP, NF1),	/* PCIE_XCLKREQ4 */
	PAD_CFG_NF(GPP_H23, NONE, DEEP, NF1),	/* PCIE_XCLKREQ5 */

	/* Community 1 - Gpio Group GPP_D */
	PAD_CFG_NF(GPP_D5, NONE, DEEP, NF1),	/* PCIE_XCLKREQ0 */
	PAD_CFG_NF(GPP_D6, NONE, DEEP, NF1),	/* PCIE_XCLKREQ1 */
	PAD_CFG_NF(GPP_D7, NONE, DEEP, NF1),	/* PCIE_XCLKREQ2 */
	PAD_CFG_NF(GPP_D8, NONE, DEEP, NF1),	/* PCIE_XCLKREQ3 */
	PAD_CFG_GPI(GPP_D10, NONE, PLTRST),
	PAD_CFG_GPI(GPP_D12, NONE, PLTRST),
	PAD_CFG_NF(GPP_D17, NONE, DEEP, NF1),	/* UART1_RXD */
	PAD_CFG_NF(GPP_D18, NONE, DEEP, NF1),	/* UART1_TXD */

	/* Community 2 - Gpio Group GPD */
	PAD_CFG_GPI(GPD7, NONE, PLTRST),

	/* Community 4 - Gpio Group GPP_C */
	PAD_CFG_GPI(GPP_C2, NONE, PLTRST),
	PAD_CFG_GPI(GPP_C5, NONE, DEEP),

	/* Community 4 - Gpio Group GPP_F */
	PAD_CFG_GPI(GPP_F0, NONE, PLTRST),
	PAD_CFG_GPO(GPP_F4, 0, PLTRST),
	PAD_CFG_GPI(GPP_F7, NONE, PLTRST),
	PAD_CFG_GPO(GPP_F17, 1, PLTRST),
	PAD_CFG_NF(GPP_F19, NONE, DEEP, NF1),	/* PCIE_XCLKREQ6 */

	/* Community 4 - Gpio Group GPP_E */
	PAD_CFG_GPI(GPP_E6, NONE, PLTRST),
	PAD_CFG_GPI(GPP_E8, NONE, PLTRST),
	PAD_CFG_NF(GPP_E9, NONE, DEEP, NF1),	/* USB2 OC0 pin */
	PAD_CFG_GPI(GPP_E14, NONE, PLTRST),
	PAD_CFG_GPI(GPP_E19, NONE, PLTRST),
	PAD_CFG_GPI(GPP_E21, NONE, PLTRST),
	PAD_CFG_GPO(GPP_E22, 0, PLTRST),

	/* Community 5 - Gpio Group GPP_R */
	PAD_CFG_GPI(GPP_R2, NONE, PLTRST),

};

void variant_configure_gpio_pads(void)
{
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}
