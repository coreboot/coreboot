/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017-2018 Intel Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <assert.h>
#include <device/pci_def.h>
#include <intelblocks/gpio.h>
#include <intelblocks/lpss.h>
#include <intelblocks/pcr.h>
#include <intelblocks/uart.h>
#include <soc/iomap.h>
#include <soc/pch.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <string.h>

/* Serial IO UART controller legacy mode */
#define PCR_SERIAL_IO_GPPRVRW7		0x618
#define PCR_SIO_PCH_LEGACY_UART(idx)	(1 << (idx))

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

void soc_uart_set_legacy_mode(void)
{
	pcr_write32(PID_SERIALIO, PCR_SERIAL_IO_GPPRVRW7,
		PCR_SIO_PCH_LEGACY_UART(CONFIG_UART_FOR_CONSOLE));
	/*
	 * Dummy read after setting any of GPPRVRW7.
	 * Required for UART 16550 8-bit Legacy mode to become active
	 */
	lpss_clk_read(UART_BASE(CONFIG_UART_FOR_CONSOLE));
}

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
	default:
		printk(BIOS_ERR, "Invalid UART console index\n");
		return NULL;
	}
}
