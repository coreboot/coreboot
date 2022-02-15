/* SPDX-License-Identifier: GPL-2.0-only */

#include <bl_uapp/bl_syscall_public.h>
#include <amdblocks/uart.h>
#include <types.h>

static void *uart_bars[FCH_UART_ID_MAX];

uintptr_t get_uart_base(unsigned int idx)
{
	uint32_t err;

	if (idx >= ARRAY_SIZE(uart_bars))
		return 0;

	if (uart_bars[idx])
		return (uintptr_t)uart_bars[idx];

	err = svc_map_fch_dev(FCH_IO_DEVICE_UART, idx, 0, &uart_bars[idx]);
	if (err) {
		svc_debug_print("Failed to map UART\n");
		return 0;
	}

	return (uintptr_t)uart_bars[idx];
}
