/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <amdblocks/smbus.h>
#include <console/console.h>
#include <soc/southbridge.h>
#include <soc/uart.h>

/* Before console init */
void fch_pre_init(void)
{
	enable_acpimmio_decode_pm04();
	fch_smbus_init();
	fch_enable_cf9_io();
	fch_enable_legacy_io();
	enable_aoac_devices();

	/*
	 * On reset Range_0 defaults to enabled. We want to start with a clean
	 * slate to not have things unexpectedly enabled.
	 */
	clear_uart_legacy_config();

	if (CONFIG(AMD_SOC_CONSOLE_UART))
		set_uart_config(CONFIG_UART_FOR_CONSOLE);
}

/* After console init */
void fch_early_init(void)
{
	fch_print_pmxc0_status();
}
