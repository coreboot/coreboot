/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2018 Intel Corp.
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

/*
 * The sole purpose of this driver is to avoid BAR to be changed during
 * resource allocation. Since configuration space is just 32 bytes it
 * shouldn't cause any fragmentation.
 */

#include <assert.h>
#include <intelblocks/uart.h>
#include <soc/gpio.h>
#include <soc/pci_devs.h>
#include <string.h>

/* UART pad configuration. Support RXD and TXD for now. */
const struct uart_gpio_pad_config uart_gpio_pads[] = {
#if IS_ENABLED(CONFIG_SOC_INTEL_GLK)
	{
		.console_index = 0,
		.gpios = {
			PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_60, NATIVE, DEEP, NF1,
				HIZCRx1, DISPUPD), /* LPSS_UART0_RXD */
			PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_61, NATIVE, DEEP, NF1,
				HIZCRx1, DISPUPD), /* LPSS_UART0_TXD */

		},
	},
	{
		.console_index = 2,
		.gpios = {
			PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_64, NATIVE, DEEP, NF1,
				HIZCRx1, DISPUPD), /* LPSS_UART2_RXD */
			PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_65, NATIVE, DEEP, NF1,
				HIZCRx1, DISPUPD), /* LPSS_UART2_TXD */
		},
	},
#else
	{
		.console_index = 1,
		.gpios = {
			PAD_CFG_NF(GPIO_42, NATIVE, DEEP, NF1), /* UART1 RX */
			PAD_CFG_NF(GPIO_43, NATIVE, DEEP, NF1), /* UART1 TX */
		},
	},
	{
		.console_index = 2,
		.gpios = {
			PAD_CFG_NF(GPIO_46, NATIVE, DEEP, NF1), /* UART2 RX */
			PAD_CFG_NF(GPIO_47, NATIVE, DEEP, NF1), /* UART2 TX */
		},
	},
#endif
};

const int uart_max_index = ARRAY_SIZE(uart_gpio_pads);

struct device *soc_uart_console_to_device(int uart_console)
{
	/*
	 * if index is valid, this function will return corresponding structure
	 * for uart console else will return NULL.
	 */
	switch (uart_console) {
	case 0:
		return (struct device *)PCH_DEV_UART0;
	case 1:
		return (struct device *)PCH_DEV_UART1;
	case 2:
		return (struct device *)PCH_DEV_UART2;
	case 3:
		return (struct device *)PCH_DEV_UART3;
	default:
		printk(BIOS_ERR, "Invalid UART console index\n");
		return NULL;
	}
}
