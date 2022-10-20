/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Update for Morgana */

#include <amdblocks/gpio.h>
#include <amdblocks/uart.h>
#include <commonlib/helpers.h>
#include <device/mmio.h>
#include <soc/aoac_defs.h>
#include <soc/gpio.h>
#include <soc/iomap.h>
#include <soc/southbridge.h>
#include <soc/uart.h>
#include <types.h>

static const struct soc_uart_ctrlr_info uart_info[] = {
	[0] =	{ APU_UART0_BASE, FCH_AOAC_DEV_UART0, "FUR0", {
			PAD_NF(GPIO_143, UART0_TXD, PULL_NONE),
			PAD_NF(GPIO_141, UART0_RXD, PULL_NONE),
		} },
	[1] =	{ APU_UART1_BASE, FCH_AOAC_DEV_UART1, "FUR1", {
			PAD_NF(GPIO_140, UART1_TXD, PULL_NONE),
			PAD_NF(GPIO_142, UART1_RXD, PULL_NONE),
		} },
	[2] =	{ APU_UART2_BASE, FCH_AOAC_DEV_UART2, "FUR2", {
			PAD_NF(GPIO_138, UART2_TXD, PULL_NONE),
			PAD_NF(GPIO_136, UART2_RXD, PULL_NONE),
		} },
	[3] =	{ APU_UART3_BASE, FCH_AOAC_DEV_UART3, "FUR3", {
			PAD_NF(GPIO_135, UART3_TXD, PULL_NONE),
			PAD_NF(GPIO_137, UART3_RXD, PULL_NONE),
		} },
	[4] =	{ APU_UART4_BASE, FCH_AOAC_DEV_UART4, "FUR4", {
			PAD_NF(GPIO_156, UART4_TXD, PULL_NONE),
			PAD_NF(GPIO_155, UART4_RXD, PULL_NONE),
		} },
};

const struct soc_uart_ctrlr_info *soc_get_uart_ctrlr_info(size_t *num_ctrlrs)
{
	*num_ctrlrs = ARRAY_SIZE(uart_info);
	return uart_info;
}

void clear_uart_legacy_config(void)
{
	write16p(FCH_LEGACY_UART_DECODE, 0);
}
