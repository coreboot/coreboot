/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/aoac.h>
#include <soc/aoac_defs.h>
#include <types.h>

#if CONFIG(AMD_SOC_CONSOLE_UART) && FCH_AOAC_UART_FOR_CONSOLE == -1
# error Unsupported UART_FOR_CONSOLE chosen
#endif

static const unsigned int aoac_devs[] = {
#if CONFIG(AMD_SOC_CONSOLE_UART)
	FCH_AOAC_UART_FOR_CONSOLE,
#endif
};

const unsigned int *soc_get_aoac_devices(size_t *num)
{
	*num = ARRAY_SIZE(aoac_devs);
	return aoac_devs;
}
