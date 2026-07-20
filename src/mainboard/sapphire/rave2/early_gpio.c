/* SPDX-License-Identifier: GPL-2.0-only */

#include <gpio.h>
#include "gpio.h"

static const struct soc_amd_gpio gpio_set_stage_reset[] = {
	/* WAKE_L */
	PAD_NF(GPIO_2, WAKE_L, PULL_UP),
	/* NVMe Reset pin */
	PAD_GPO(GPIO_8, HIGH),
	/* ESPI_CS_L */
	PAD_NF(GPIO_30, ESPI_CS_L, PULL_NONE),
	/* ESPI_SOC_CLK */
	PAD_NF(GPIO_77, SPI1_CLK, PULL_NONE),
	/* ESPI_DATA0 */
	PAD_NF(GPIO_81, SPI1_DAT0, PULL_NONE),
	/* ESPI_DATA1 */
	PAD_NF(GPIO_80, SPI1_DAT1, PULL_NONE),
	/* ESPI_DATA2 */
	PAD_NF(GPIO_68, SPI1_DAT2, PULL_NONE),
	/* ESPI_DATA3 */
	PAD_NF(GPIO_69, SPI1_DAT3, PULL_NONE),
	/* ESPI_ALERT_L */
	PAD_NF(GPIO_22, ESPI_ALERT_D1, PULL_NONE),
	/* PCIE_RST0_L */
	PAD_NFO(GPIO_26, PCIE_RST0_L, HIGH),
	/* PCIE_RST1_L */
	PAD_NFO(GPIO_27, PCIE_RST1_L, HIGH),
	/* UART0_RXD */
	PAD_NF(GPIO_141, UART0_RXD, PULL_NONE),
	/* UART0_TXD */
	PAD_NF(GPIO_143, UART0_TXD, PULL_NONE),
};

void mainboard_program_early_gpios(void)
{
	gpio_configure_pads(gpio_set_stage_reset, ARRAY_SIZE(gpio_set_stage_reset));
}
