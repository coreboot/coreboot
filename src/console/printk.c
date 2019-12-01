/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * blatantly copied from linux/kernel/printk.c
 */

#include <console/cbmem_console.h>
#include <console/console.h>
#include <console/streams.h>
#include <console/vtxprintf.h>
#include <smp/spinlock.h>
#include <smp/node.h>
#include <stddef.h>
#include <trace.h>

#if (!defined(__PRE_RAM__) && CONFIG(HAVE_ROMSTAGE_CONSOLE_SPINLOCK)) || !CONFIG(HAVE_ROMSTAGE_CONSOLE_SPINLOCK)
DECLARE_SPIN_LOCK(console_lock)
#endif

void do_putchar(unsigned char byte)
{
	console_tx_byte(byte);
}

static void wrap_putchar(unsigned char byte, void *data)
{
	console_tx_byte(byte);
}

static void wrap_putchar_cbmemc(unsigned char byte, void *data)
{
	__cbmemc_tx_byte(byte);
}

int do_vprintk(int msg_level, const char *fmt, va_list args)
{
	int i, log_this;

	if (CONFIG(SQUELCH_EARLY_SMP) && ENV_ROMSTAGE_OR_BEFORE && !boot_cpu())
		return 0;

	log_this = console_log_level(msg_level);
	if (log_this < CONSOLE_LOG_FAST)
		return 0;

	DISABLE_TRACE;
#ifdef __PRE_RAM__
#if CONFIG(HAVE_ROMSTAGE_CONSOLE_SPINLOCK)
	spin_lock(romstage_console_lock());
#endif
#else
	spin_lock(&console_lock);
#endif

	if (log_this == CONSOLE_LOG_FAST) {
		i = vtxprintf(wrap_putchar_cbmemc, fmt, args, NULL);
	} else {
		i = vtxprintf(wrap_putchar, fmt, args, NULL);
		console_tx_flush();
	}

#ifdef __PRE_RAM__
#if CONFIG(HAVE_ROMSTAGE_CONSOLE_SPINLOCK)
	spin_unlock(romstage_console_lock());
#endif
#else
	spin_unlock(&console_lock);
#endif
	ENABLE_TRACE;

	return i;
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
