/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/aoac.h>
#include <soc/aoac_defs.h>
#include <soc/southbridge.h>
#include <delay.h>

#define FCH_AOAC_UART_FOR_CONSOLE \
		(CONFIG_UART_FOR_CONSOLE == 0 ? FCH_AOAC_DEV_UART0 \
		: CONFIG_UART_FOR_CONSOLE == 1 ? FCH_AOAC_DEV_UART1 \
		: CONFIG_UART_FOR_CONSOLE == 2 ? FCH_AOAC_DEV_UART2 \
		: -1)
#if CONFIG(AMD_SOC_CONSOLE_UART) && FCH_AOAC_UART_FOR_CONSOLE == -1
# error Unsupported UART_FOR_CONSOLE chosen
#endif

void wait_for_aoac_enabled(unsigned int dev)
{
	while (!is_aoac_device_enabled(dev))
		udelay(100);
}

void enable_aoac_devices(void)
{
	if (CONFIG(AMD_SOC_CONSOLE_UART))
		power_on_aoac_device(FCH_AOAC_UART_FOR_CONSOLE);

	if (CONFIG(AMD_SOC_CONSOLE_UART))
		wait_for_aoac_enabled(FCH_AOAC_UART_FOR_CONSOLE);
}
