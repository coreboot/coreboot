/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __OPTIPLEX_3050_EARLY_GPIO_H__
#define __OPTIPLEX_3050_EARLY_GPIO_H__

static const struct pad_config early_gpio_table[] = {
	PAD_CFG_NF(GPP_C8, NONE, DEEP, NF1),	/* UART0_RXD */
	PAD_CFG_NF(GPP_C9, NONE, DEEP, NF1),	/* UART0_TXD */
};

#endif
