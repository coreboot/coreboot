/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <cbmem.h>
#include <console/console.h>
#include <console/streams.h>
#include <console/uart.h>
#include <program_loading.h>
#include <soc/clock.h>
#include <soc/sdram.h>

void main(void)
{
	console_init();

	/* TODO: Follow Section 6.3 (FSBL) of the FU540 manual */

	clock_init();

	// re-initialize UART
	if (CONFIG(CONSOLE_SERIAL))
		uart_init(CONFIG_UART_FOR_CONSOLE);

	sdram_init();

	cbmem_initialize_empty();

	run_ramstage();
}
