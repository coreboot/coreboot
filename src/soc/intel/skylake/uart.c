/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/pci_def.h>
#include <gpio.h>
#include <intelblocks/lpss.h>
#include <intelblocks/pcr.h>
#include <intelblocks/uart.h>
#include <soc/bootblock.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>

const struct uart_controller_config uart_ctrlr_config[] = {
	{
		.console_index = 0,
		.devfn = PCH_DEVFN_UART0,
		.gpios = {
			PAD_CFG_NF(GPP_C8, NONE, DEEP, NF1), /* UART0 RX */
			PAD_CFG_NF(GPP_C9, NONE, DEEP, NF1), /* UART0 TX */
		},
	},
	{
		.console_index = 1,
		.devfn = PCH_DEVFN_UART1,
		.gpios = {
			PAD_CFG_NF(GPP_C12, NONE, DEEP, NF1), /* UART1 RX */
			PAD_CFG_NF(GPP_C13, NONE, DEEP, NF1), /* UART1 TX */
		},
	},
	{
		.console_index = 2,
		.devfn = PCH_DEVFN_UART2,
		.gpios = {
			PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1), /* UART2 RX */
			PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1), /* UART2 TX */
		},
	}
};

const int uart_max_index = ARRAY_SIZE(uart_ctrlr_config);
