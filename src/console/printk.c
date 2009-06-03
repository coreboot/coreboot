/*
 *  blantantly copied from linux/kernel/printk.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *
 */

#include <stdarg.h>
#include <smp/spinlock.h>
#include <console/console.h>

/* Keep together for sysctl support */

unsigned int console_loglevel = DEFAULT_CONSOLE_LOGLEVEL;

void display(char*);
extern int vtxprintf(void (*)(unsigned char), const char *, va_list);

static spinlock_t console_lock = SPIN_LOCK_UNLOCKED;

int do_printk(int msg_level, const char *fmt, ...)
{
	va_list args;
	int i;

	if (msg_level >= console_loglevel) {
		return 0;
	}

	spin_lock(&console_lock);

	va_start(args, fmt);
	i = vtxprintf(console_tx_byte, fmt, args);
	va_end(args);

	console_tx_flush();

	spin_unlock(&console_lock);

	return i;
}
