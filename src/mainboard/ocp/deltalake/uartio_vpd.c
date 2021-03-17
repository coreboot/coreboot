/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <drivers/vpd/vpd.h>
#include <console/uart.h>
#include <string.h>

#include "vpd.h"

unsigned int get_uart_for_console(void)
{
	int val_int = COREBOOT_UART_IO_DEFAULT;

	if (vpd_get_int(COREBOOT_UART_IO, VPD_RW_THEN_RO, (int *const) &val_int)) {
		if (val_int > 3)
			val_int = COREBOOT_UART_IO_DEFAULT;
	}
	return val_int;
}
