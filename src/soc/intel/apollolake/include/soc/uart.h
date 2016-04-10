/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corp.
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_APOLLOLAKE_UART_H_
#define _SOC_APOLLOLAKE_UART_H_

/* Clock is 100MHz * (M / N).*/
#define UART_CLK		0x200
# define UART_CLK_UPDATE	(1 << 31)
# define UART_CLK_DIV_N(n)	(((n) & 0x7fff) << 16)
# define UART_CLK_DIV_M(m)	(((m) & 0x7fff) << 1)
# define UART_CLK_EN		(1 << 0)
#define UART_RESET		0x204
# define UART_RESET_DMA_EN	(1 << 2)
# define UART_RESET_UART_EN	(3 << 0)

void lpss_console_uart_init(void);

/* Initialize the console UART including the pads for the configured UART. */
void soc_console_uart_init(void);

#endif /* _SOC_APOLLOLAKE_UART_H_ */
