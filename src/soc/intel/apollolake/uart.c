/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * The sole purpose of this driver is to avoid BAR to be changed during
 * resource allocation. Since configuration space is just 32 bytes it
 * shouldn't cause any fragmentation.
 */

#include <console/console.h>
#include <intelblocks/uart.h>
#include <soc/gpio.h>
#include <soc/pci_devs.h>

/* UART pad configuration. Support RXD and TXD for now. */
const struct uart_gpio_pad_config uart_gpio_pads[] = {
#if CONFIG(SOC_INTEL_GEMINILAKE)
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
		.console_index = 0,
		.gpios = {
			PAD_CFG_NF(GPIO_38, NATIVE, DEEP, NF1), /* UART0 RX */
			PAD_CFG_NF(GPIO_39, NATIVE, DEEP, NF1), /* UART0 TX */
		},
	},
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
	case 3:
		return pcidev_path_on_root(PCH_DEVFN_UART3);
	default:
		printk(BIOS_ERR, "Invalid UART console index\n");
		return NULL;
	}
}
