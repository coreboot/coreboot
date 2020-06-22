/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <soc/gpio.h>
#include <stdlib.h>
#include <boardid.h>
#include <variant/gpio.h>

static const struct soc_amd_gpio early_gpio_table[] = {
	/* H1_FCH_INT_ODL */
	PAD_INT(GPIO_3, PULL_UP, EDGE_LOW, STATUS),
	/* I2C3_SCL - H1 */
	PAD_NF(GPIO_19, I2C3_SCL, PULL_UP),
	/* I2C3_SDA - H1 */
	PAD_NF(GPIO_20, I2C3_SDA, PULL_UP),
	/* PCIE_RST0_L - Fixed timings */
	PAD_NF(GPIO_26, PCIE_RST_L, PULL_NONE),
	/* FCH_ESPI_EC_CS_L */
	PAD_NF(GPIO_30, ESPI_CS_L, PULL_NONE),
	/* ESPI_ALERT_L (may be unused) */
	PAD_NF(GPIO_108, ESPI_ALERT_L, PULL_UP),
	/* UART0_RXD - DEBUG */
	PAD_NF(GPIO_136, UART0_RXD, PULL_NONE),
	/* UART0_TXD - DEBUG */
	PAD_NF(GPIO_138, UART0_TXD, PULL_NONE),
};

const __weak struct soc_amd_gpio *variant_early_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}
