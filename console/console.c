#include <arch/types.h>
#include <arch/hlt.h>
#include <console/console.h>
#include <uart8250.h>

// FIXME: we need this for varargs
#include <stdarg.h>

int vtxprintf(void (*)(unsigned char, void *arg), 
		void *arg, const char *, va_list);


static int console_loglevel(void)
{
	return CONFIG_DEFAULT_CONSOLE_LOGLEVEL;
}

void console_tx_byte(unsigned char byte, void *arg)
{
	if (byte == '\n') {
		uart8250_tx_byte(TTYSx_BASE, '\r');
#if defined(CONFIG_CONSOLE_PREFIX) && (CONFIG_CONSOLE_PREFIX == 1)
		uart8250_tx_byte(TTYSx_BASE, '\n');
		uart8250_tx_byte(TTYSx_BASE, '(');
		uart8250_tx_byte(TTYSx_BASE, 'L');
		uart8250_tx_byte(TTYSx_BASE, 'B');
		uart8250_tx_byte(TTYSx_BASE, ')');
		uart8250_tx_byte(TTYSx_BASE, ' ');
		return;
#endif
	}

	uart8250_tx_byte(TTYSx_BASE, byte);
}

int printk(int msg_level, const char *fmt, ...)
{
	va_list args;
	int i;

	if (msg_level >= console_loglevel()) {
		return 0;
	}

	va_start(args, fmt);
	i = vtxprintf(console_tx_byte, (void *)0, fmt, args);
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
	printk(BIOS_EMERG, str);
	do {
		hlt();
	} while (1);
}
