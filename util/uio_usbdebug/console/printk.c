/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdio.h>
#include <stdarg.h>
#include <console/console.h>

int do_printk(int msg_level, const char *const fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vfprintf(stderr, fmt, args);
	va_end(args);

	return i;
}
