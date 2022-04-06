/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/gpio.h>

#include "gpio.h"

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* SMB_CLK */
	PAD_CFG_NF(GPP_C0, NONE, DEEP, NF1),
	/* SMB_DATA */
	PAD_CFG_NF(GPP_C1, NONE, DEEP, NF1),

	/* EC_SMI_N */
	PAD_CFG_GPI(GPP_E7, NONE, DEEP),

	/* PCH HSID */
	PAD_CFG_GPI(GPP_A8, NONE, DEEP),
	PAD_CFG_GPI(GPP_F19, NONE, DEEP),
	PAD_CFG_GPI(GPP_H19, NONE, DEEP),
	PAD_CFG_GPI(GPP_H23, NONE, DEEP),

	/* UART0 RX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* UART0 TX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),
	/* UART1 RX */
	PAD_CFG_NF(GPP_D17, NONE, DEEP, NF1),
	/* UART1 TX */
	PAD_CFG_NF(GPP_D18, NONE, DEEP, NF1),
};

void configure_early_gpio_pads(void)
{
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}
