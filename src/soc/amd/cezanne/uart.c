/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/gpio_banks.h>
#include <amdblocks/uart.h>
#include <commonlib/helpers.h>
#include <device/mmio.h>
#include <soc/gpio.h>
#include <soc/southbridge.h>
#include <soc/uart.h>
#include <types.h>

static const struct _uart_info {
	uintptr_t base;
	struct soc_amd_gpio mux[2];
} uart_info[] = {
	[0] = { APU_UART0_BASE, {
			PAD_NF(GPIO_143, UART0_TXD, PULL_NONE),
			PAD_NF(GPIO_141, UART0_RXD, PULL_NONE),
	} },
	[1] = { APU_UART1_BASE, {
			PAD_NF(GPIO_140, UART1_TXD, PULL_NONE),
			PAD_NF(GPIO_142, UART1_RXD, PULL_NONE),
	} },
};

uintptr_t get_uart_base(unsigned int idx)
{
	if (idx >= ARRAY_SIZE(uart_info))
		return 0;

	return uart_info[idx].base;
}

void clear_uart_legacy_config(void)
{
	write16((void *)FCH_LEGACY_UART_DECODE, 0);
}

void set_uart_config(unsigned int idx)
{
	if (idx >= ARRAY_SIZE(uart_info))
		return;

	program_gpios(uart_info[idx].mux, 2);
}
