/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/uart.h>
#include <option.h>
#include <security/vboot/misc.h>
#include <types.h>

#if __CONSOLE_SERIAL_RUNTIME_DECISION__

bool console_serial_enabled(void)
{
	if (CONFIG(CONSOLE_SERIAL_ONLY_PAYLOAD) &&
	    (!CONFIG(VBOOT) || !vboot_logic_executed() ||
	     vboot_recovery_mode_enabled() ||
	     vboot_developer_mode_enabled()))
		return true;

	if (CONFIG(CONSOLE_SERIAL_RUNTIME)) {
		static bool cached;
		static bool enabled;

		if (!cached) {
			/*
			 * Normalize to a boolean. With USE_UEFI_VARIABLE_STORE,
			 * get_uint_option() is stubbed in separate verstage and
			 * postcar and returns the CONSOLE_SERIAL fallback there.
			 */
			enabled = !!get_uint_option("serial_console",
						    CONFIG(CONSOLE_SERIAL));
			cached = true;
		}

		if (enabled)
			return true;
	}

	return false;
}

#endif /* __CONSOLE_SERIAL_RUNTIME_DECISION__ */

#if __CONSOLE_SERIAL_ACTIVE__

void __uart_init(void)
{
	if (!console_serial_enabled())
		return;

	uart_init(get_uart_for_console());
}

void __uart_tx_byte(u8 data)
{
	if (!console_serial_enabled())
		return;

	uart_tx_byte(get_uart_for_console(), data);
}

void __uart_tx_flush(void)
{
	if (!console_serial_enabled())
		return;

	uart_tx_flush(get_uart_for_console());
}

#endif /* __CONSOLE_SERIAL_ACTIVE__ */
