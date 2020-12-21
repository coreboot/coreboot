/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/gpio.h>
#include <variant/gpio.h>

static const struct pad_config early_gpio_table[] = {
/* GPP_C8    UART0A_RXD           0x0000005044000702 */ PAD_CFG_NF(GPP_C8, NONE, DEEP, NF1),
/* GPP_C9    UART0A_TXD           0x0000005144000700 */ PAD_CFG_NF(GPP_C9, NONE, DEEP, NF1),
/* GPP_C20   UART2_RXD            0x0000005c44000500 */ PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1),
/* GPP_C21   UART2_TXD            0x0000005d44000600 */ PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1),
};

void variant_configure_early_gpios(void)
{
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}
