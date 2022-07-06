/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <boardid.h>
#include <soc/gpio.h>

/* Pad configuration in ramstage */
static const struct pad_config gpio_table[] = {
	/* ToDo: Fill gpio configuration */
	/* H08 : UART0_RXD ==> UART_DBG_TX_SOC_RX */
	PAD_CFG_NF(GPP_H08, NONE, DEEP, NF1),
	/* H09 : UART0_TXD ==> UART_DBG_RX_SOC_TX */
	PAD_CFG_NF(GPP_H09, NONE, DEEP, NF1),
};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* ToDo: Fill early gpio configuration */
	/* H08 : UART0_RXD ==> UART_DBG_TX_SOC_RX */
	PAD_CFG_NF(GPP_H08, NONE, DEEP, NF1),
	/* H09 : UART0_TXD ==> UART_DBG_RX_SOC_TX */
	PAD_CFG_NF(GPP_H09, NONE, DEEP, NF1),
};

static const struct pad_config romstage_gpio_table[] = {
	/* ToDo: Fill romstage gpio configuration */
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

/* Create the stub for romstage gpio, typically use for power sequence */
const struct pad_config *variant_romstage_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(romstage_gpio_table);
	return romstage_gpio_table;
}

static const struct cros_gpio cros_gpios[] = {
};

DECLARE_WEAK_CROS_GPIOS(cros_gpios);
