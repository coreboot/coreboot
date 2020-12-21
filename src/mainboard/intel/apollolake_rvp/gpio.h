/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <soc/gpio.h>

static const struct pad_config early_gpio_table[] = {
	PAD_CFG_NF(GPIO_46, NATIVE, DEEP, NF1),	/* UART2 RX */
	PAD_CFG_NF(GPIO_47, NATIVE, DEEP, NF1),	/* UART2 TX */
};
