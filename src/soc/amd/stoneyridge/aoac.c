/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/aoac.h>
#include <delay.h>
#include <soc/aoac_defs.h>
#include <soc/southbridge.h>
#include <types.h>

/*
 * Table of devices that need their AOAC registers enabled and waited
 * upon (usually about .55 milliseconds). Instead of individual delays
 * waiting for each device to become available, a single delay will be
 * executed.
 */
static const unsigned int aoac_devs[] = {
	FCH_AOAC_DEV_UART0 + CONFIG_UART_FOR_CONSOLE * 2,
	FCH_AOAC_DEV_AMBA,
	FCH_AOAC_DEV_I2C0,
	FCH_AOAC_DEV_I2C1,
	FCH_AOAC_DEV_I2C2,
	FCH_AOAC_DEV_I2C3,
};

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
