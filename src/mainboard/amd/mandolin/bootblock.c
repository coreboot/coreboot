/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <amdblocks/lpc.h>
#include <superio/smsc/sio1036/sio1036.h>
#include "gpio.h"

#define SERIAL_DEV PNP_DEV(CONFIG_SUPERIO_ADDR_BASE, SIO1036_SP1)

void bootblock_mainboard_early_init(void)
{
	mainboard_program_early_gpios();

	if (CONFIG(SUPERIO_SMSC_SIO1036)) {
		if (CONFIG_SUPERIO_ADDR_BASE == 0x4e) {
			lpc_enable_sio_decode(LPC_SELECT_SIO_4E4F);
		} else {
			// set up 16 byte wide I/O range window for the super IO
			lpc_set_wideio_range(CONFIG_SUPERIO_ADDR_BASE & ~0xF, 16);
		}
		lpc_enable_decode(DECODE_ENABLE_SERIAL_PORT0 << CONFIG_UART_FOR_CONSOLE);
		sio1036_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	}
}
