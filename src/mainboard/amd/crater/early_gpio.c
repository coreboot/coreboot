/* SPDX-License-Identifier: GPL-2.0-only */

#include <gpio.h>
#include "gpio.h"

/* GPIO pins used by coreboot should be initialized in bootblock */

static const struct soc_amd_gpio gpio_set_stage_reset[] = {
	PAD_NF(GPIO_0, PWR_BTN_L, PULL_UP),
	PAD_NF(GPIO_1, SYS_RESET_L, PULL_UP),
	PAD_NF(GPIO_2, WAKE_L, PULL_UP),
	PAD_GPI(GPIO_3, PULL_UP),
	/* TPM CS */
	PAD_NF(GPIO_129, KBRST_L, PULL_NONE),
	/* SPI_ROM_REQ */
	PAD_NF(GPIO_67, SPI_ROM_REQ, PULL_NONE),
	/* SPI_ROM_GNT */
	PAD_NF(GPIO_76, SPI_ROM_GNT, PULL_NONE),
	/* LPC_PME */
	PAD_NF(GPIO_22, LPC_PME_L, PULL_NONE),

	/* Deassert PCIe Reset lines */
	/* PCIE_RST0_L */
	PAD_NFO(GPIO_26, PCIE_RST_L, HIGH),
	/* PCIE_RST1_L */
	PAD_NFO(GPIO_27, PCIE_RST1_L, HIGH),
	/* M2_SSD0_RST */
	PAD_GPO(GPIO_24, HIGH),
	/* DEVSLP1 */
	PAD_NFO(GPIO_6, DEVSLP1, LOW),

	/*I2S SP/BT Audio & Record*/
	PAD_NF(GPIO_8, ACP_I2S_LRCLK, PULL_DOWN),
	PAD_NF(GPIO_7, ACP_I2S_SDIN, PULL_DOWN),

	/*MDIO0_SCL*/
	PAD_NF(GPIO_10, MDIO0_SCL, PULL_DOWN),
	/*MDIO0_SDA*/
	PAD_NF(GPIO_40, MDIO0_SDA, PULL_DOWN),
	/*MDIO1_SCL*/
	PAD_NF(GPIO_9, MDIO1_SCL, PULL_DOWN),
	/*MDIO1_SDA*/
	PAD_NF(GPIO_23, MDIO1_SDA, PULL_DOWN),


	/* Enable UART 0 */
	/* UART0_RXD */
	PAD_NF(GPIO_141, UART0_RXD, PULL_NONE),
	/* UART0_TXD */
	PAD_NF(GPIO_143, UART0_TXD, PULL_NONE),
	/* FANOUT0 */
	PAD_NF(GPIO_85, FANOUT0, PULL_NONE),

	/* I2C0 SCL */
	PAD_NF(GPIO_145, I2C0_SCL, PULL_NONE),
	/* I2C0 SDA */
	PAD_NF(GPIO_146, I2C0_SDA, PULL_NONE),
	/* I2C1 SCL */
	PAD_NF(GPIO_147, I2C1_SCL, PULL_NONE),
	/* I2C1 SDA */
	PAD_NF(GPIO_148, I2C1_SDA, PULL_NONE),
	/* I2C2_SCL */
	PAD_NF(GPIO_113, I2C2_SCL, PULL_NONE),
	/* I2C2_SDA */
	PAD_NF(GPIO_114, I2C2_SDA, PULL_NONE),
	/* I2C3_SCL*/
	PAD_NF(GPIO_19, I2C3_SCL, PULL_NONE),
	/* I2C3_SDA*/
	PAD_NF(GPIO_20, I2C3_SDA, PULL_NONE),
};

void mainboard_program_early_gpios(void)
{
	gpio_configure_pads(gpio_set_stage_reset, ARRAY_SIZE(gpio_set_stage_reset));
}
