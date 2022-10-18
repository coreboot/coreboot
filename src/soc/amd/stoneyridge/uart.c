/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/aoac.h>
#include <amdblocks/gpio.h>
#include <amdblocks/uart.h>
#include <soc/aoac_defs.h>
#include <soc/gpio.h>
#include <soc/iomap.h>
#include <soc/southbridge.h>
#include <types.h>

static const struct soc_uart_ctrlr_info uart_info[] = {
	[0] =	{ APU_UART0_BASE, FCH_AOAC_DEV_UART0, "FUR0", {
			PAD_NF(GPIO_138, UART0_TXD, PULL_NONE),
			PAD_NF(GPIO_136, UART0_RXD, PULL_NONE),
		} },
	[1] =	{ APU_UART1_BASE, FCH_AOAC_DEV_UART1, "FUR1", {
			PAD_NF(GPIO_143, UART1_TXD, PULL_NONE),
			PAD_NF(GPIO_141, UART1_RXD, PULL_NONE),
		} },
};

uintptr_t get_uart_base(unsigned int idx)
{
	if (idx >= ARRAY_SIZE(uart_info))
		return 0;

	return uart_info[idx].base;
}

void set_uart_config(unsigned int idx)
{
	if (idx >= ARRAY_SIZE(uart_info))
		return;

	gpio_configure_pads(uart_info[idx].mux, 2);
}
