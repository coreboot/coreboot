/*
 *  blatantly copied from linux/kernel/printk.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *
 */

#include <smp/node.h>
#include <smp/spinlock.h>
#include <console/vtxprintf.h>
#include <console/console.h>
#include <trace.h>

DECLARE_SPIN_LOCK(console_lock)

int do_printk(int msg_level, const char *fmt, ...)
{
	va_list args;
	int i;

	if (!console_show(msg_level))
		return 0;

#if CONFIG_SQUELCH_EARLY_SMP && defined(__PRE_RAM__)
	if (!boot_cpu())
		return 0;
#endif

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

#if CONFIG_CHROMEOS
void do_vtxprintf(const char *fmt, va_list args)
{
	vtxprintf(console_tx_byte, fmt, args);
	console_tx_flush();
}
#endif
