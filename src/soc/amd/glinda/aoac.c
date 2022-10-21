/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Update for Glinda */

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
		: CONFIG_UART_FOR_CONSOLE == 3 ? FCH_AOAC_DEV_UART3 \
		: CONFIG_UART_FOR_CONSOLE == 4 ? FCH_AOAC_DEV_UART4 \
		: -1)
#if CONFIG(AMD_SOC_CONSOLE_UART) && FCH_AOAC_UART_FOR_CONSOLE == -1
# error Unsupported UART_FOR_CONSOLE chosen
#endif

/*
 * Table of devices that need their AOAC registers enabled and waited
 * upon (usually about .55 milliseconds). Instead of individual delays
 * waiting for each device to become available, a single delay will be
 * executed.  The console UART is handled separately from this table.
 *
 * TODO: Find out which I2C controllers we really need to enable here.
 */
const static unsigned int aoac_devs[] = {
	FCH_AOAC_DEV_AMBA,
	FCH_AOAC_DEV_I2C0,
	FCH_AOAC_DEV_I2C1,
	FCH_AOAC_DEV_I2C2,
	FCH_AOAC_DEV_I2C3,
	FCH_AOAC_DEV_ESPI,
};

void wait_for_aoac_enabled(unsigned int dev)
{
	while (!is_aoac_device_enabled(dev))
		udelay(100);
}

void enable_aoac_devices(void)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(aoac_devs); i++)
		power_on_aoac_device(aoac_devs[i]);

	if (CONFIG(AMD_SOC_CONSOLE_UART))
		power_on_aoac_device(FCH_AOAC_UART_FOR_CONSOLE);

	/* Wait for AOAC devices to indicate power and clock OK */
	for (i = 0; i < ARRAY_SIZE(aoac_devs); i++)
		wait_for_aoac_enabled(aoac_devs[i]);

	if (CONFIG(AMD_SOC_CONSOLE_UART))
		wait_for_aoac_enabled(FCH_AOAC_UART_FOR_CONSOLE);
}
