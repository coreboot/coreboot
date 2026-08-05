/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/aoac.h>
#include <delay.h>
#include <soc/aoac_defs.h>
#include <soc/southbridge.h>
#include <types.h>

#if CONFIG(AMD_SOC_CONSOLE_UART) && FCH_AOAC_UART_FOR_CONSOLE == -1
# error Unsupported UART_FOR_CONSOLE chosen
#endif

/*
 * Devices that need their AOAC registers enabled during early FCH init.
 * Console UART is included when AMD_SOC_CONSOLE_UART is selected.
 */
static const unsigned int aoac_devs[] = {
	FCH_AOAC_DEV_AMBA,
	FCH_AOAC_DEV_I2C0,
	FCH_AOAC_DEV_I2C1,
	FCH_AOAC_DEV_I2C2,
	FCH_AOAC_DEV_I2C3,
#if CONFIG(AMD_SOC_CONSOLE_UART)
	FCH_AOAC_UART_FOR_CONSOLE,
#endif
};

void wait_for_aoac_enabled(unsigned int dev)
{
	while (!is_aoac_device_enabled(dev))
		udelay(100);
}

void enable_aoac_devices(void)
{
	bool status;
	int i;

	for (i = 0; i < ARRAY_SIZE(aoac_devs); i++)
		power_on_aoac_device(aoac_devs[i]);

	/* Wait for AOAC devices to indicate power and clock OK */
	do {
		udelay(100);
		status = true;
		for (i = 0; i < ARRAY_SIZE(aoac_devs); i++)
			status &= is_aoac_device_enabled(aoac_devs[i]);
	} while (!status);
}
