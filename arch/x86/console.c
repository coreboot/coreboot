#include <arch/types.h>
#include <arch/hlt.h>
#include <console/loglevel.h>
#include "config.h"

// FIXME: we need this for varargs
#include <stdarg.h>

#if MAXIMUM_CONSOLE_LOGLEVEL <= BIOS_DEBUG
#define debug(msg_level, fmt, arg...)   printk(msg_level, fmt, ##arg)
#endif

#ifndef LINUXBIOS_EXTRA_VERSION
#define LINUXBIOS_EXTRA_VERSION ""
#endif

/* printk's without a loglevel use this.. */
#define DEFAULT_MESSAGE_LOGLEVEL 4 /* BIOS_WARNING */

extern int vtxprintf(void (*)(unsigned char), const char *, va_list);
extern void uart8250_tx_byte(unsigned, unsigned char);

int console_loglevel(void)
{
	return CONFIG_DEFAULT_CONSOLE_LOGLEVEL;
}

void console_tx_byte(unsigned char byte)
{
	if (byte == '\n')
		uart8250_tx_byte(TTYS0_BASE, '\r');
	uart8250_tx_byte(TTYS0_BASE, byte);
}

int printk(int msg_level, const char *fmt, ...)
{
	va_list args;
	int i;

	if (msg_level >= console_loglevel()) {
		return 0;
	}

	va_start(args, fmt);
	i = vtxprintf(console_tx_byte, fmt, args);
	va_end(args);

	return i;
}

void console_init(void)
{
	static const char console_test[] = 
		"\n\nLinuxBIOS-"
		LINUXBIOS_VERSION
		LINUXBIOS_EXTRA_VERSION
		" "
		LINUXBIOS_BUILD
		" starting...\n";

	printk(BIOS_INFO, console_test);
}

void die(const char *str)
{
	printk(BIOS_EMERG,str);
	do {
		hlt();
	} while(1);
}
