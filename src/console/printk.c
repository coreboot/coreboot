/*
 *  blatantly copied from linux/kernel/printk.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *
 */

#include <console/console.h>
#include <console/streams.h>
#include <console/vtxprintf.h>
#include <smp/spinlock.h>
#include <smp/node.h>
#include <stddef.h>
#include <trace.h>

DECLARE_SPIN_LOCK(console_lock)

void do_putchar(unsigned char byte)
{
	console_tx_byte(byte);
}

static void wrap_putchar(unsigned char byte, void *data)
{
	do_putchar(byte);
}

int do_printk(int msg_level, const char *fmt, ...)
{
	va_list args;
	int i;

	if (!console_log_level(msg_level))
		return 0;

#if IS_ENABLED (CONFIG_SQUELCH_EARLY_SMP) && defined(__PRE_RAM__)
	if (!boot_cpu())
		return 0;
#endif

	DISABLE_TRACE;
	spin_lock(&console_lock);

	va_start(args, fmt);
	i = vtxprintf(wrap_putchar, fmt, args, NULL);
	va_end(args);

	console_tx_flush();

	spin_unlock(&console_lock);
	ENABLE_TRACE;

	return i;
}

#if IS_ENABLED (CONFIG_CHROMEOS)
void do_vtxprintf(const char *fmt, va_list args)
{
	vtxprintf(wrap_putchar, fmt, args, NULL);
	console_tx_flush();
}
#endif /* CONFIG_CHROMEOS */
