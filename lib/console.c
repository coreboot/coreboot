#include <types.h>
#include <cpu.h>
#include <console.h>
#include <uart8250.h>
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

	if (msg_level > console_loglevel()) {
		return 0;
	}

	va_start(args, fmt);
	i = vtxprintf(console_tx_byte, (void *)0, fmt, args);
	va_end(args);

	return i;
}

/**
 * Print a nice banner so we know what step we died on. 
 *
 * @param level The printk level (e.g. BIOS_EMERG)
 * @param s String to put in the middle of the banner
 */

void banner(int level, const char *s)
{
	int i;
	/* 10 = signs and a space. */
	printk(level, "========== ");
	for(i = 11; *s; i++, s++)
		printk(level, "%c", *s);
	/* trailing space */
	printk(level, " ");
	i++;
	/* fill it up to 80 columns */
	for(;  i < 80; i++)
		printk(level, "=");
	printk(level, "\n");
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

/**
 *  Halt and loop due to a fatal error. 
 *  There have been several iterations of this function. 
 *  The first simply did a hlt(). Doing a hlt() can make jtag debugging
 *  very difficult as one can not break into a hlt instruction on some CPUs. 
 *  Second was to do a console_tx_byte of a NULL character. 
 *  A number of concerns were raised about doing this idea. 
 *  Third idea was to do an inb from port 0x80, the POST port. That design 
 *  makes us very CPU-specific. 
 *  The fourth idea was just POSTING the same
 *  code over and over. That would erase the most recent POST code, 
 *  hindering diagnosis. 
 *
 *  For now, for lack of a good alternative, 
 *  we will continue to call console_tx_byte. We call with a NULL since
 *  it will clear any FIFOs in the path and won't clutter up the output,
 *  since NULL doesn't print a visible character on most terminal 
 *  emulators. 
 *
 *  @param str A string to print for the error
 *
 */
void die(const char *str)
{
	printk(BIOS_EMERG, str);
	while (1)
		console_tx_byte(0, (void *)0);
}
