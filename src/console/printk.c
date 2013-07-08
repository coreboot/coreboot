/*
 *  blatantly copied from linux/kernel/printk.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *
 */

#include <smp/spinlock.h>
#include <console/vtxprintf.h>
#include <console/console.h>
#include <trace.h>

int console_loglevel = CONFIG_DEFAULT_CONSOLE_LOGLEVEL;
int default_console_loglevel = CONFIG_DEFAULT_CONSOLE_LOGLEVEL;

DECLARE_SPIN_LOCK(console_lock)

int do_printk(int msg_level, const char *fmt, ...)
{
	va_list args;
	int i;

	if (msg_level > console_loglevel) {
		return 0;
	}

	DISABLE_TRACE;
	spin_lock(&console_lock);

	va_start(args, fmt);
	i = vtxprintf(console_tx_byte, fmt, args);
	va_end(args);

	console_tx_flush();

	spin_unlock(&console_lock);
	ENABLE_TRACE;

	return i;
}
