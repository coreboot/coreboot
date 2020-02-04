/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <console/console.h>
#include <console/vtxprintf.h>
#include <vb2_api.h>

/*
 * vboot callbacks implemented by coreboot -- necessary for making general API
 * calls when CONFIG_VBOOT_LIB is enabled.  For callbacks specific to verstage
 * (CONFIG_VBOOT), please see vboot_logic.c.
 */

void vb2ex_printf(const char *func, const char *fmt, ...)
{
	va_list args;

	if (func)
		printk(BIOS_INFO, "VB2:%s() ", func);

	va_start(args, fmt);
	vprintk(BIOS_INFO, fmt, args);
	va_end(args);
}

void vb2ex_abort(void)
{
	die("vboot has aborted execution; exit\n");
}
