/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <halt.h>
#include <stdarg.h>

/*
 * The method should be overwritten in mainboard directory to signal that a
 * fatal error had occurred. On boards that do share the same EC and where the
 * EC is capable of controlling LEDs or a buzzer the method can be overwritten
 * in EC directory instead.
 */
__weak void die_notify(void)
{
}

/* Report a fatal error */
void __noreturn die(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	vprintk(BIOS_EMERG, fmt, args);
	va_end(args);

	die_notify();
	halt();
}
