/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <soc/soc.h>
#include <soc/spi.h>
#include <soc/uart.h>

void bootblock_mainboard_early_init(void)
{
	if (CONFIG(BOOTBLOCK_CONSOLE)) {
		if (!uart_is_enabled(CONFIG_UART_FOR_CONSOLE))
			uart_setup(CONFIG_UART_FOR_CONSOLE, CONFIG_TTYS0_BAUD);
	}
}

static void configure_spi_flash(void)
{
	/* FIXME: Only tested on EM100 Pro */
	spi_init_custom(0, // bus
			25000000, // speed Hz
			0, // idle low disabled
			0, // zero idle cycles between transfers
			0, // MSB first
			0, // Chip select 0
			1); // assert is high
}

void bootblock_mainboard_init(void)
{
	configure_spi_flash();
}
