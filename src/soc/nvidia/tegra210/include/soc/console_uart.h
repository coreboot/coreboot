/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Andre Heider <a.heider@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __SOC_NVIDIA_TEGRA210_INCLUDE_SOC_CONSOLE_UART_H__
#define __SOC_NVIDIA_TEGRA210_INCLUDE_SOC_CONSOLE_UART_H__

#include <soc/clock.h>

enum console_uart_id {
	UART_ID_NONE	= -1,
	UART_ID_A	= 0,
	UART_ID_B	= 1,
	UART_ID_C	= 2,
	UART_ID_D	= 3,
	UART_ID_E	= 4,
};

static inline enum console_uart_id console_uart_get_id(void)
{
	if (IS_ENABLED(CONFIG_CONSOLE_SERIAL_TEGRA210_UARTA))
		return UART_ID_A;

	if (IS_ENABLED(CONFIG_CONSOLE_SERIAL_TEGRA210_UARTB))
		return UART_ID_B;

	if (IS_ENABLED(CONFIG_CONSOLE_SERIAL_TEGRA210_UARTC))
		return UART_ID_C;

	if (IS_ENABLED(CONFIG_CONSOLE_SERIAL_TEGRA210_UARTD))
		return UART_ID_D;

	if (IS_ENABLED(CONFIG_CONSOLE_SERIAL_TEGRA210_UARTE))
		return UART_ID_E;

	return UART_ID_NONE;
}

static inline void *console_uart_clk_rst_reg(void)
{
	switch (console_uart_get_id()) {
	case UART_ID_NONE:
		return NULL;
	case UART_ID_A:
		return CLK_RST_REG(clk_src_uarta);
	case UART_ID_B:
		return CLK_RST_REG(clk_src_uartb);
	case UART_ID_C:
		return CLK_RST_REG(clk_src_uartc);
	case UART_ID_D:
		return CLK_RST_REG(clk_src_uartd);
	case UART_ID_E:
		return CLK_RST_REG(clk_src_uarte);
	}

	return NULL;
}

static inline uint32_t console_uart_clk_src_dev_id(void)
{
	switch (console_uart_get_id()) {
	case UART_ID_NONE:
		return 0;
	case UART_ID_A:
		return CLK_SRC_DEV_ID(UARTA, PLLP);
	case UART_ID_B:
		return CLK_SRC_DEV_ID(UARTB, PLLP);
	case UART_ID_C:
		return CLK_SRC_DEV_ID(UARTC, PLLP);
	case UART_ID_D:
		return CLK_SRC_DEV_ID(UARTD, PLLP);
	case UART_ID_E:
		return CLK_SRC_DEV_ID(UARTE, PLLP);
	}

	return 0;
}

static inline void console_uart_clock_enable_clear_reset(void)
{
	switch (console_uart_get_id()) {
	case UART_ID_NONE:
		return;
	case UART_ID_A:
		clock_enable_clear_reset_l(CLK_L_UARTA);
		break;
	case UART_ID_B:
		clock_enable_clear_reset_l(CLK_L_UARTB);
		break;
	case UART_ID_C:
		clock_enable_clear_reset_h(CLK_H_UARTC);
		break;
	case UART_ID_D:
		clock_enable_clear_reset_u(CLK_U_UARTD);
		break;
	case UART_ID_E:
		clock_enable_clear_reset_u(CLK_U_UARTE);
		break;
	}
}

#endif /* __SOC_NVIDIA_TEGRA210_INCLUDE_SOC_CONSOLE_UART_H__ */
