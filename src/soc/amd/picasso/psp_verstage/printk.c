/* SPDX-License-Identifier: GPL-2.0-only */

#include <bl_uapp/bl_syscall_public.h>
#include <console/console.h>
#include <console/cbmem_console.h>
#include <console/streams.h>
#include <stdarg.h>

void console_hw_init(void)
{
	__cbmemc_init();
}

int printk(int msg_level, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vprintk(msg_level, fmt, args);
	va_end(args);

	return i;
}

int vprintk(int msg_level, const char *fmt, va_list args)
{
	int i, cnt, log_this;
	char buf[256];

	log_this = console_log_level(msg_level);
	if (log_this < CONSOLE_LOG_FAST)
		return 0;

	cnt = vsnprintf(buf, sizeof(buf), fmt, args);
	for (i = 0; i < cnt; i++)
		__cbmemc_tx_byte(buf[i]);
	svc_debug_print(buf);
	return i;
}
