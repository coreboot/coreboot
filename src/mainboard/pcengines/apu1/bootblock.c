/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/pnp_type.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct5104d/nct5104d.h>

#define SIO_PORT 0x2e
#define SERIAL1_DEV PNP_DEV(SIO_PORT, NCT5104D_SP1)
#define SERIAL2_DEV PNP_DEV(SIO_PORT, NCT5104D_SP2)

void bootblock_mainboard_early_init(void)
{
	if (CONFIG_UART_FOR_CONSOLE == 1)
		nuvoton_enable_serial(SERIAL2_DEV, CONFIG_TTYS0_BASE);
	else if (CONFIG_UART_FOR_CONSOLE == 0)
		nuvoton_enable_serial(SERIAL1_DEV, CONFIG_TTYS0_BASE);
}
