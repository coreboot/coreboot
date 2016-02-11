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
 */

#include <console/uart.h>
#include <device/pci.h>
#include <soc/gpio.h>
#include <soc/uart.h>
#include <soc/pci_devs.h>

static void lpss_uart_write(uint16_t reg, uint32_t val)
{
	uintptr_t base = CONFIG_CONSOLE_UART_BASE_ADDRESS | reg;
	write32((void *)base, val);
}

void lpss_console_uart_init(void)
{
	uint32_t clk_sel;
	device_t uart = _LPSS_PCI_DEV(UART, CONFIG_UART_FOR_CONSOLE & 3);

	if (CONFIG_UART_FOR_CONSOLE > 2)
		return;

	/* Enable BAR0 for the UART -- this is where the 8250 registers hide */
	pci_write_config32(uart, PCI_BASE_ADDRESS_0,
			   CONFIG_CONSOLE_UART_BASE_ADDRESS);

	/* Enable memory access and bus master */
	pci_write_config32(uart, PCI_COMMAND,
			   PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER);

        /* Take UART out of reset */
	lpss_uart_write(UART_RESET, UART_RESET_UART_EN);

	/* These values get us a 1.836 MHz clock (ideally we want 1.843 MHz) */
	clk_sel = UART_CLK_DIV_N(0x7fff) | UART_CLK_DIV_M(0x025a);
	/* Set M and N divisor inputs and enable clock */
	lpss_uart_write(UART_CLK, clk_sel | UART_CLK_UPDATE);
	lpss_uart_write(UART_CLK, clk_sel | UART_CLK_EN);

}

uintptr_t uart_platform_base(int idx)
{
	return (CONFIG_CONSOLE_UART_BASE_ADDRESS);
}

unsigned int uart_platform_refclk(void)
{
	/* That's within 0.5% of the actual value we've set earlier */
	return 115200 * 16;
}
