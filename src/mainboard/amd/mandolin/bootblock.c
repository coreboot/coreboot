/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <amdblocks/lpc.h>
#include <superio/smsc/sio1036/sio1036.h>
#include "gpio.h"

#define SERIAL_DEV PNP_DEV(0x4e, SIO1036_SP1)

void bootblock_mainboard_early_init(void)
{
	mainboard_program_early_gpios();

	if (CONFIG(SUPERIO_SMSC_SIO1036)) {
		lpc_enable_sio_decode(LPC_SELECT_SIO_4E4F);
		lpc_enable_decode(DECODE_ENABLE_SERIAL_PORT0 << CONFIG_UART_FOR_CONSOLE);
		sio1036_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	}
}
