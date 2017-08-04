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
#include <intelblocks/uart.h>
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

uintptr_t uart_platform_base(int idx)
{
	return CONFIG_CONSOLE_UART_BASE_ADDRESS;
}

static const struct pad_config uart_gpios[] = {
#if IS_ENABLED(CONFIG_SOC_INTEL_GLK)
	PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_60, NATIVE, DEEP, NF1, HIZCRx1,
		DISPUPD), /* LPSS_UART0_RXD */
	PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_61, NATIVE, DEEP, NF1, HIZCRx1,
		DISPUPD), /* LPSS_UART0_TXD */
	PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_64, NATIVE, DEEP, NF1, HIZCRx1,
		DISPUPD), /* LPSS_UART2_RXD */
	PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_65, NATIVE, DEEP, NF1, HIZCRx1,
		DISPUPD), /* LPSS_UART2_TXD */
#else
	PAD_CFG_NF(GPIO_42, NATIVE, DEEP, NF1),		/* UART1 RX */
	PAD_CFG_NF(GPIO_43, NATIVE, DEEP, NF1),		/* UART1 TX */
	PAD_CFG_NF(GPIO_46, NATIVE, DEEP, NF1),		/* UART2 RX */
	PAD_CFG_NF(GPIO_47, NATIVE, DEEP, NF1),		/* UART2 TX */
#endif
};

void pch_uart_init(void)
{
	uintptr_t base = CONFIG_CONSOLE_UART_BASE_ADDRESS;
	device_t uart = _PCH_DEV(UART, CONFIG_UART_FOR_CONSOLE & 3);

	/* Get a 0-based pad index. See invalid_uart_for_console() above. */
	const int pad_index = CONFIG_UART_FOR_CONSOLE - 1;

	if (invalid_uart_for_console())
		return;

	/* Configure the 2 pads per UART. */
	gpio_configure_pads(&uart_gpios[pad_index * 2], 2);

	/* Program UART2 BAR0, command, reset and clock register */
	uart_common_init(uart, base);

}
