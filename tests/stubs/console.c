/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <stdarg.h>
#include <stdio.h>
#include <tests/test.h>

#ifndef TEST_PRINT
#define TEST_PRINT 0
#endif

int printk(int msg_level, const char *fmt, ...)
{
#if TEST_PRINT
	va_list v;
	va_start(v, fmt);
	vprint_message(fmt, v);
	va_end(v);
#endif
	return 0;
}

int vprintk(int msg_level, const char *fmt, va_list args)
{
#if TEST_PRINT
	vprint_message(fmt, args);
#endif
	return 0;
}

int console_log_level(int msg_level)
{
	return 0;
}
