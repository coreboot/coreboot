/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * blatantly copied from linux/kernel/printk.c
 */

#include <console/cbmem_console.h>
#include <console/console.h>
#include <console/streams.h>
#include <console/vtxprintf.h>
#include <smp/spinlock.h>
#include <smp/node.h>
#include <timer.h>

DECLARE_SPIN_LOCK(console_lock)

#define TRACK_CONSOLE_TIME (!ENV_SMM && CONFIG(HAVE_MONOTONIC_TIMER))

static struct mono_time mt_start, mt_stop;
static long console_usecs;

static void console_time_run(void)
{
	if (TRACK_CONSOLE_TIME && boot_cpu())
		timer_monotonic_get(&mt_start);
}

static void console_time_stop(void)
{
	if (TRACK_CONSOLE_TIME && boot_cpu()) {
		timer_monotonic_get(&mt_stop);
		console_usecs += mono_time_diff_microseconds(&mt_start, &mt_stop);
	}
}

void console_time_report(void)
{
	if (!TRACK_CONSOLE_TIME)
		return;

	printk(BIOS_DEBUG, "BS: " ENV_STRING " times (exec / console): total (unknown) / %ld ms\n",
		DIV_ROUND_CLOSEST(console_usecs, USECS_PER_MSEC));
}

long console_time_get_and_reset(void)
{
	if (!TRACK_CONSOLE_TIME)
		return 0;

	long elapsed = console_usecs;
	console_usecs = 0;
	return elapsed;
}

void do_putchar(unsigned char byte)
{
	console_time_run();
	console_tx_byte(byte);
	console_time_stop();
}

static void wrap_putchar(unsigned char byte, void *data)
{
	console_tx_byte(byte);
}

static void wrap_putchar_cbmemc(unsigned char byte, void *data)
{
	__cbmemc_tx_byte(byte);
}

int vprintk(int msg_level, const char *fmt, va_list args)
{
	int i, log_this;

	if (CONFIG(SQUELCH_EARLY_SMP) && ENV_ROMSTAGE_OR_BEFORE && !boot_cpu())
		return 0;

	log_this = console_log_level(msg_level);
	if (log_this < CONSOLE_LOG_FAST)
		return 0;

	spin_lock(&console_lock);

	console_time_run();

	if (log_this == CONSOLE_LOG_FAST) {
		i = vtxprintf(wrap_putchar_cbmemc, fmt, args, NULL);
	} else {
		i = vtxprintf(wrap_putchar, fmt, args, NULL);
		console_tx_flush();
	}

	console_time_stop();

	spin_unlock(&console_lock);

	return i;
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
