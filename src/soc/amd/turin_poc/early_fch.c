/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/espi.h>
#include <amdblocks/lpc.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/aoac.h>
#include <amdblocks/pmlib.h>
#include <amdblocks/uart.h>
#include <soc/southbridge.h>
#include <soc/uart.h>

/* Before console init */
void fch_pre_init(void)
{
	fch_enable_cf9_io();

	enable_aoac_devices();
	/*
	 * On reset Range_0 defaults to enabled. We want to start with a clean
	 * slate to not have things unexpectedly enabled.
	 */
	clear_uart_legacy_config();

	if (CONFIG(AMD_SOC_CONSOLE_UART))
		set_uart_config(CONFIG_UART_FOR_CONSOLE);

	configure_espi_with_mb_hook();
}

/* After console init */
void fch_early_init(void)
{
}
