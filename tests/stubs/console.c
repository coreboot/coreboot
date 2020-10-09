/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <stdarg.h>
#include <stdio.h>

int do_printk(int msg_level, const char *fmt, ...)
{
	return 0;
}

int do_vprintk(int msg_level, const char *fmt, va_list args)
{
	return 0;
}

int console_log_level(int msg_level)
{
	return 0;
}
