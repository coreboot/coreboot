/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
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

	/*
	 * Flush console before changing clock/UART divisor to prevent garbage
	 * being printed.
	 */
	console_tx_flush();

	clock_init();

	// re-initialize UART
	#if (IS_ENABLED(CONFIG_CONSOLE_SERIAL))
		uart_init(CONFIG_UART_FOR_CONSOLE);
	#endif

	sdram_init();

	run_ramstage();
}
