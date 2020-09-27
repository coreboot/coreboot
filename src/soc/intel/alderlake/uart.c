/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on Intel Alder Lake Processor PCH Datasheet
 * Document number: 621483
 * Chapter number: 9
 */

#include <console/console.h>
#include <device/pci_def.h>
#include <intelblocks/gpio.h>
#include <intelblocks/lpss.h>
#include <intelblocks/pcr.h>
#include <intelblocks/uart.h>
#include <soc/iomap.h>
#include <soc/pch.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>

const struct uart_gpio_pad_config uart_gpio_pads[] = {
	{
		.console_index = 0,
		.gpios = {
			PAD_CFG_NF(GPP_C8, NONE, DEEP, NF1), /* UART0 RX */
			PAD_CFG_NF(GPP_C9, NONE, DEEP, NF1), /* UART0 TX */
		},
	},
	{
		.console_index = 1,
		.gpios = {
			PAD_CFG_NF(GPP_C12, NONE, DEEP, NF1), /* UART1 RX */
			PAD_CFG_NF(GPP_C13, NONE, DEEP, NF1), /* UART1 TX */
		},
	},
	{
		.console_index = 2,
		.gpios = {
			PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1), /* UART2 RX */
			PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1), /* UART2 TX */
		},
	}
};

const int uart_max_index = ARRAY_SIZE(uart_gpio_pads);

DEVTREE_CONST struct device *soc_uart_console_to_device(int uart_console)
{
	/*
	 * if index is valid, this function will return corresponding structure
	 * for uart console else will return NULL.
	 */
	switch (uart_console) {
	case 0:
		return pcidev_path_on_root(PCH_DEVFN_UART0);
	case 1:
		return pcidev_path_on_root(PCH_DEVFN_UART1);
	case 2:
		return pcidev_path_on_root(PCH_DEVFN_UART2);
	default:
		printk(BIOS_ERR, "Invalid UART console index\n");
		return NULL;
	}
}
