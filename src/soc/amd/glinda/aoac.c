/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Update for Glinda */

#include <amdblocks/aoac.h>
#include <device/device.h>
#include <soc/aoac_defs.h>
#include <static.h>
#include <types.h>

#if CONFIG(AMD_SOC_CONSOLE_UART) && FCH_AOAC_UART_FOR_CONSOLE == -1
# error Unsupported UART_FOR_CONSOLE chosen
#endif

/*
 * Devices that need their AOAC registers enabled during early FCH init.
 * Console UART is included when AMD_SOC_CONSOLE_UART is selected.
 *
 * TODO: Find out which I2C controllers we really need to enable here.
 */
static const unsigned int aoac_devs[] = {
	FCH_AOAC_DEV_AMBA,
	FCH_AOAC_DEV_I2C0,
	FCH_AOAC_DEV_I2C1,
	FCH_AOAC_DEV_I2C2,
	FCH_AOAC_DEV_I2C3,
	FCH_AOAC_DEV_ESPI,
#if CONFIG(AMD_SOC_CONSOLE_UART)
	FCH_AOAC_UART_FOR_CONSOLE,
#endif
};

/* Same as aoac_devs[], plus HID2 for boards that wire it up. */
static const unsigned int aoac_devs_hid2[] = {
	FCH_AOAC_DEV_AMBA,
	FCH_AOAC_DEV_I2C0,
	FCH_AOAC_DEV_I2C1,
	FCH_AOAC_DEV_I2C2,
	FCH_AOAC_DEV_I2C3,
	FCH_AOAC_DEV_ESPI,
#if CONFIG(AMD_SOC_CONSOLE_UART)
	FCH_AOAC_UART_FOR_CONSOLE,
#endif
	FCH_AOAC_DEV_HID2,
};

const unsigned int *soc_get_aoac_devices(size_t *num)
{
	/* HID2 is only wired up on some boards; devicetree tells us which. */
	if (is_dev_enabled(DEV_PTR(hid2))) {
		*num = ARRAY_SIZE(aoac_devs_hid2);
		return aoac_devs_hid2;
	}

	*num = ARRAY_SIZE(aoac_devs);
	return aoac_devs;
}
