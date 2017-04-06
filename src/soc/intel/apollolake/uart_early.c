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

#include <console/uart.h>
#include <device/pci.h>
#include <intelblocks/lpss.h>
#include <soc/gpio.h>
#include <soc/uart.h>
#include <soc/pci_devs.h>

static inline int invalid_uart_for_console(void)
{
	/* There are actually only 2 UARTS, and they are named UART1 and
	 * UART2. They live at pci functions 1 and 2 respectively. */
	if (CONFIG_UART_FOR_CONSOLE > 2 || CONFIG_UART_FOR_CONSOLE < 1)
		return 1;
	return 0;
}

void lpss_console_uart_init(void)
{
	uintptr_t base = CONFIG_CONSOLE_UART_BASE_ADDRESS;
	device_t uart = _PCH_DEV(UART, CONFIG_UART_FOR_CONSOLE & 3);

	if (invalid_uart_for_console())
		return;

	/* Enable BAR0 for the UART -- this is where the 8250 registers hide */
	pci_write_config32(uart, PCI_BASE_ADDRESS_0, base);

	/* Enable memory access and bus master */
	pci_write_config32(uart, PCI_COMMAND,
			   PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER);

	/* Take UART out of reset */
	lpss_reset_release(base);

	/*
	 * Set M and N divisor inputs and enable clock. These values
	 * get us a 1.836 MHz clock (ideally we want 1.843 MHz)
	 */
	lpss_clk_update(base, 0x025a, 0x7fff);

}

uintptr_t uart_platform_base(int idx)
{
	return CONFIG_CONSOLE_UART_BASE_ADDRESS;
}

static const struct pad_config uart_gpios[] = {
	PAD_CFG_NF(GPIO_42, NATIVE, DEEP, NF1),		/* UART1 RX */
	PAD_CFG_NF(GPIO_43, NATIVE, DEEP, NF1),		/* UART1 TX */
	PAD_CFG_NF(GPIO_46, NATIVE, DEEP, NF1),		/* UART2 RX */
	PAD_CFG_NF(GPIO_47, NATIVE, DEEP, NF1),		/* UART2 TX */
};

void soc_console_uart_init(void)
{
	/* Get a 0-based pad index. See invalid_uart_for_console() above. */
	const int pad_index = CONFIG_UART_FOR_CONSOLE - 1;

	if (invalid_uart_for_console())
		return;

	/* Configure the 2 pads per UART. */
	gpio_configure_pads(&uart_gpios[pad_index * 2], 2);

	lpss_console_uart_init();
}
