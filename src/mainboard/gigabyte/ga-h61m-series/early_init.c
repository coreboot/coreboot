/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <northbridge/intel/sandybridge/raminit.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8728f/it8728f.h>

#define SUPERIO_GPIO PNP_DEV(0x2e, IT8728F_GPIO)
#define SERIAL_DEV PNP_DEV(0x2e, 0x01)

void bootblock_mainboard_early_init(void)
{
	if (!CONFIG(NO_UART_ON_SUPERIO)) {
		/* Enable serial port */
		ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	}

	/* Disable SIO WDT which kicks in DualBIOS */
	ite_reg_write(SUPERIO_GPIO, 0xEF, 0x7E);
}
