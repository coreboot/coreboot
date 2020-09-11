/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <console/uart.h>
#include <commonlib/bsd/helpers.h>
#include <soc/addressmap.h>
#include <soc/clock.h>

uintptr_t uart_platform_base(unsigned int idx)
{
	if (idx < 2)
		return FU540_UART(idx);
	else
		return 0;
}

unsigned int uart_platform_refclk(void)
{
	return clock_get_tlclk_khz() * KHz;
}
