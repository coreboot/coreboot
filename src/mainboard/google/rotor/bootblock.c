/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Marvell, Inc.
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

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <arch/io.h>
#include <bootblock_common.h>
#include <console/console.h>
#include <soc/clock.h>
#include <soc/pinmux.h>

#include "board.h"

struct mvmap2315_pinmux board_pinmux[] = {
	PINMUX_APB_UART0_TXD,
	PINMUX_APB_UART0_RXD,
	PINMUX_APB_UART1_TXD,
	PINMUX_APB_UART1_RXD,
};

static void pinmux_init(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(board_pinmux); i++)
		set_pinmux(board_pinmux[i]);
}

void bootblock_mainboard_early_init(void)
{
	/* enable APB/UART0 clock */
	setbits_le32(&mvmap2315_apmu_clk->uartclk0_clkgenconfig,
		     MVMAP2315_UART_CLK_EN);

	/* enable APB/UART1 clock */
	setbits_le32(&mvmap2315_apmu_clk->uartclk1_clkgenconfig,
		     MVMAP2315_UART_CLK_EN);

	pinmux_init();
}
