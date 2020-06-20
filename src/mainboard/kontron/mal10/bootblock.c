/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <intelblocks/lpc_lib.h>
#include <ec/kontron/kempld/kempld.h>
#include <variant/gpio.h>

static void init_cpld(void)
{
	/* Set up LPC decoding for CPLD I/O port ranges */
	lpc_open_pmio_window(0x0A80, 2);

	/* Enable console serial ports */
	lpc_io_setup_comm_a_b();
	kempld_enable_uart_for_console();
}

void bootblock_mainboard_early_init(void)
{
	variant_early_gpio_configure();
	init_cpld();
}

void bootblock_mainboard_init(void)
{
}
