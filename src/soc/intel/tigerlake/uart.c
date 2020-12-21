/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on Intel Tiger Lake Processor PCH Datasheet
 * Document number: 575857
 * Chapter number: 9
 */

#include <commonlib/helpers.h>
#include <soc/pci_devs.h>

const unsigned int uart_devices[] = {
	PCH_DEVFN_UART0,
	PCH_DEVFN_UART1,
	PCH_DEVFN_UART2,
};

const int uart_devices_size = ARRAY_SIZE(uart_devices);
