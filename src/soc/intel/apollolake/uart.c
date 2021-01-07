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

const struct uart_controller_config uart_ctrlr_config[] = {
#if CONFIG(SOC_INTEL_GEMINILAKE)
	{
		.console_index = 0,
		.devfn = PCH_DEVFN_UART0,
		.gpios = {
			PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_60, NATIVE, DEEP, NF1,
				HIZCRx1, DISPUPD), /* LPSS_UART0_RXD */
			PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_61, NATIVE, DEEP, NF1,
				HIZCRx1, DISPUPD), /* LPSS_UART0_TXD */

		},
	},
	{
		.console_index = 2,
		.devfn = PCH_DEVFN_UART2,
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
		.devfn = PCH_DEVFN_UART0,
		.gpios = {
			PAD_CFG_NF(GPIO_38, NATIVE, DEEP, NF1), /* UART0 RX */
			PAD_CFG_NF(GPIO_39, NATIVE, DEEP, NF1), /* UART0 TX */
		},
	},
	{
		.console_index = 1,
		.devfn = PCH_DEVFN_UART1,
		.gpios = {
			PAD_CFG_NF(GPIO_42, NATIVE, DEEP, NF1), /* UART1 RX */
			PAD_CFG_NF(GPIO_43, NATIVE, DEEP, NF1), /* UART1 TX */
		},
	},
	{
		.console_index = 2,
		.devfn = PCH_DEVFN_UART2,
		.gpios = {
			PAD_CFG_NF(GPIO_46, NATIVE, DEEP, NF1), /* UART2 RX */
			PAD_CFG_NF(GPIO_47, NATIVE, DEEP, NF1), /* UART2 TX */
		},
	},
#endif
};

const int uart_max_index = ARRAY_SIZE(uart_ctrlr_config);
