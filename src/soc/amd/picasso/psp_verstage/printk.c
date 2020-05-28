/* SPDX-License-Identifier: GPL-2.0-only */

#include <bl_uapp/bl_syscall_public.h>
#include <console/console.h>
#include <console/streams.h>
#include <stdarg.h>

void console_hw_init(void)
{
	// Nothing to init for svc_debug_print
}

int do_printk(int msg_level, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = do_vprintk(msg_level, fmt, args);
	va_end(args);

	return i;
}

int do_vprintk(int msg_level, const char *fmt, va_list args)
{
	int i, log_this;
	char buf[256];

	log_this = console_log_level(msg_level);
	if (log_this < CONSOLE_LOG_FAST)
		return 0;

	i = vsnprintf(buf, sizeof(buf), fmt, args);
	svc_debug_print(buf);
	return i;
}
