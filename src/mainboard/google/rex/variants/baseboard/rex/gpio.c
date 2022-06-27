/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <soc/gpio.h>

/* Pad configuration in ramstage */
static const struct pad_config gpio_table[] = {
	/* ToDo: Fill gpio configuration */
	/* H8 : UART0_RXD ==> UART_DBG_TX_SOC_RX */
	PAD_CFG_NF(GPP_H8, NONE, DEEP, NF1),
	/* H9 : UART0_TXD ==> UART_DBG_RX_SOC_TX */
	PAD_CFG_NF(GPP_H9, NONE, DEEP, NF1),
};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* ToDo: Fill early gpio configuration */
	/* H8 : UART0_RXD ==> UART_DBG_TX_SOC_RX */
	PAD_CFG_NF(GPP_H8, NONE, DEEP, NF1),
	/* H9 : UART0_TXD ==> UART_DBG_RX_SOC_TX */
	PAD_CFG_NF(GPP_H9, NONE, DEEP, NF1),
};

const struct pad_config *__weak variant_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}

const struct pad_config *__weak variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}

/* Create the stub for romstage gpio, typically use for power sequence */
const struct pad_config *__weak variant_romstage_gpio_table(size_t *num)
{
	*num = 0;
	return NULL;
}

static const struct cros_gpio cros_gpios[] = {
};

DECLARE_WEAK_CROS_GPIOS(cros_gpios);
