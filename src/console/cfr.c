/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/uart.h>
#include <device/device.h>
#include <device/pnp.h>
#include <device/resource.h>
#include <drivers/option/cfr_frontend.h>
#include <types.h>

#if CONFIG(INTEL_LPSS_UART_FOR_CONSOLE)
#include <intelblocks/uart.h>
#endif

static const struct sm_object debug_level = SM_DECLARE_ENUM({
	.opt_name	= "debug_level",
	.ui_name	= "Console Log Level",
	.ui_helptext	= "Set the verbosity of the coreboot console output.",
	.default_value	= CONFIG_DEFAULT_CONSOLE_LOGLEVEL,
	.values		= (const struct sm_enum_value[]) {
				{ "Emergency",		0		},
				{ "Alert",		1		},
				{ "Critical",		2		},
				{ "Error",		3		},
				{ "Warning",		4		},
				{ "Notice",		5		},
				{ "Info",		6		},
				{ "Debug",		7		},
				{ "Spew",		8		},
				SM_ENUM_VALUE_END			},
});

/*
 * True when the board has a console UART that is usable for serial output.
 * LPSS: UART_FOR_CONSOLE device must be enabled. 8250IO defaults to y on x86
 * even without a COM port, so require built-in COM1, an explicit select under
 * NO_UART_ON_SUPERIO, or an enabled Super I/O UART in the device tree whose
 * I/O resource matches the console UART base.
 */
static bool console_uart_enabled(void)
{
#if CONFIG(INTEL_LPSS_UART_FOR_CONSOLE)
	return is_dev_enabled(uart_get_device());
#elif CONFIG(AMD_SOC_CONSOLE_UART)
	return uart_platform_base(get_uart_for_console()) != 0;
#elif CONFIG(ENABLE_BUILTIN_COM1) || \
	(CONFIG(DRIVERS_UART_8250IO) && CONFIG(NO_UART_ON_SUPERIO))
	/* Board explicitly selected a known console UART path. */
	return true;
#elif CONFIG(DRIVERS_UART_8250MEM) || CONFIG(DRIVERS_UART_8250MEM_32) || \
	CONFIG(HAVE_UART_SPECIAL)
	return uart_platform_base(get_uart_for_console()) != 0;
#elif CONFIG(DRIVERS_UART_8250IO)
	const uintptr_t base = uart_platform_base(get_uart_for_console());
	DEVTREE_CONST struct device *dev = NULL;

	if (!base)
		return false;

	while ((dev = dev_find_path(dev, DEVICE_PATH_PNP)) != NULL) {
		const struct resource *res;

		if (!is_dev_enabled(dev))
			continue;

		for (res = dev->resource_list; res; res = res->next) {
			if ((res->flags & IORESOURCE_IO) && res->base == base)
				return true;
		}
	}
	return false;
#else
	return false;
#endif
}

static void update_serial_console(struct sm_object *new)
{
	/*
	 * Only suppress visibility. Keep default_value as CONFIG(CONSOLE_SERIAL)
	 * so a false-negative UART check cannot persist serial_console=0 against
	 * a compile-time enabled serial console.
	 */
	if (!CONFIG(CONSOLE_SERIAL_RUNTIME) || !console_uart_enabled())
		new->sm_bool.flags |= CFR_OPTFLAG_SUPPRESS;
}

static const struct sm_object serial_console = SM_DECLARE_BOOL({
	.opt_name	= "serial_console",
	.ui_name	= "Serial Console",
	.ui_helptext	= "Enable coreboot serial console output. When disabled, serial"
			  " TX is skipped (faster boot if no cable is attached). Requires"
			  " CONSOLE_SERIAL_RUNTIME. Default follows CONSOLE_SERIAL.",
	.default_value	= CONFIG(CONSOLE_SERIAL),
}, WITH_CALLBACK(update_serial_console));

/*
 * Auto-collected Console form for every board with CFR. serial_console is
 * suppressed when CONSOLE_SERIAL_RUNTIME is off or the console UART is not
 * present/enabled.
 */
static const __cfr_form struct sm_obj_form console = {
	.ui_name	= "Console",
	.obj_list	= (const struct sm_object *[]) {
		&debug_level,
		&serial_console,
		NULL,
	},
};
