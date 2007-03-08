#include <arch/types.h>
#include <arch/hlt.h>
#include <console/loglevel.h>
#include <uart8250.h>

// FIXME: we need this for varargs
#include <stdarg.h>

extern int vtxprintf(void (*)(unsigned char, void *arg), void *arg,
		     const char *, va_list);

int console_loglevel(void)
{
	return CONFIG_DEFAULT_CONSOLE_LOGLEVEL;
}

void console_tx_byte(unsigned char byte, void *ignored)
{
	if (byte == '\n')
		uart8250_tx_byte(TTYSx_BASE, '\r');
	uart8250_tx_byte(TTYSx_BASE, byte);
}

int printk(int msg_level, const char *fmt, ...)
{
	va_list args;
	int i;

	if (msg_level >= console_loglevel()) {
		return 0;
	}

	i = vtxprintf(console_tx_byte, (void *)0, "(LB) ", args);

	va_start(args, fmt);
	i = vtxprintf(console_tx_byte, (void *)0, fmt, args);
	va_end(args);

	return i;
}

void console_init(void)
{
	va_list args;
	static const char console_test[] =
		"\n\nLinuxBIOS-"
		LINUXBIOS_VERSION
		LINUXBIOS_EXTRA_VERSION
		" "
		LINUXBIOS_BUILD
		" starting...\n";

	/* We don't use printk() directly here in order to avoid printing
	   the "(LB) " in the first line of LinuxBIOS output. */
	if (BIOS_INFO < console_loglevel())
		vtxprintf(console_tx_byte, (void *)0, console_test, args);
}

void die(const char *str)
{
	printk(BIOS_EMERG, str);
	do {
		hlt();
	} while (1);
}
