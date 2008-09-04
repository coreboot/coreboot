#include <types.h>
#include <cpu.h>
#include <console.h>
#include <globalvars.h>
#include <uart8250.h>
#include <stdarg.h>
#include <string.h>
#include <globalvars.h>

int vtxprintf(void (*)(unsigned char, void *arg), 
		void *arg, const char *, va_list);

/**
 * set the console log level
 * There are no invalid settings, although there are ones that 
 * do not make much sense. 
 *
 * @param level The new level
 */
void set_loglevel(unsigned int level) {
	if (level > BIOS_SPEW)
		printk(BIOS_ALWAYS, "Warning: ridiculous log level setting: %d (max %d)\n", 
			level, BIOS_SPEW);
	global_vars()->loglevel = level;
}

/**
 * get the console log level
 *
 * @return The level
 */
static unsigned int console_loglevel(void)
{
	return global_vars()->loglevel;
}

void console_loglevel_init(void)
{
	set_loglevel(CONFIG_DEFAULT_CONSOLE_LOGLEVEL);
}

#ifdef CONFIG_CONSOLE_BUFFER
struct printk_buffer *printk_buffer_addr(void)
{
	return global_vars()->printk_buffer;
}

void printk_buffer_move(void *newaddr, int newsize)
{
	struct printk_buffer *oldbuf, *newbuf;
	int copylen;
	oldbuf = printk_buffer_addr();
	newbuf = newaddr;
	newbuf->len = newsize;
	newbuf->readoffset = 0;
	/* Check for wraparound */
	if (oldbuf->writeoffset < oldbuf->readoffset) {
		/* Copy from readoffset to end of buffer. */
		copylen = oldbuf->len - oldbuf->readoffset;
	} else {
		/* Copy from readoffset to writeoffset (exclusive).*/
		copylen = oldbuf->writeoffset - oldbuf->readoffset;
	}
	if (copylen > newsize)
		copylen = newsize;
	/* If memcpy() ever uses printk we will see pretty explosions. */
	memcpy(&newbuf->buffer[0], &oldbuf->buffer[oldbuf->readoffset],
		copylen);
	newbuf->writeoffset = copylen;
	/* Check for wraparound */
	if (oldbuf->writeoffset < oldbuf->readoffset) {
		/* Copy from start of buffer to writeoffset (exclusive). */
		copylen = (copylen + oldbuf->writeoffset > newsize)
				? newsize - copylen : oldbuf->writeoffset;
		memcpy(&newbuf->buffer[newbuf->writeoffset],
			&oldbuf->buffer[0], copylen);
		newbuf->writeoffset += copylen;
	}
	global_vars()->printk_buffer = newbuf;
	return;
}

void printk_buffer_init(void)
{
	struct printk_buffer *buf = (struct printk_buffer *)PRINTK_BUF_ADDR_CAR;
	global_vars()->printk_buffer = buf;
	buf = printk_buffer_addr();
	buf->len = PRINTK_BUF_SIZE_CAR - sizeof(struct printk_buffer);
	buf->readoffset = 0;
	buf->writeoffset = 0;
	return;
}

void buffer_tx_byte(unsigned char byte, void *arg)
{
	struct printk_buffer *buf = printk_buffer_addr();
	buf->buffer[buf->writeoffset++] = byte;
	buf->writeoffset %= buf->len;
	/* Make sure writeoffset is always ahead of readoffset here. */
	if (buf->writeoffset == buf->readoffset) {
		buf->readoffset++;
		buf->readoffset %= buf->len;
	}
	return;
}
#endif

void console_tx_byte(unsigned char byte, void *arg)
{
#ifdef CONFIG_CONSOLE_BUFFER
	buffer_tx_byte(byte, arg);
#endif
#ifdef CONFIG_CONSOLE_SERIAL
	if (byte == '\n') {
		uart8250_tx_byte(TTYSx_BASE, '\r');
#ifdef CONFIG_CONSOLE_PREFIX
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
#endif
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
		"\n\ncoreboot-"
		COREBOOT_VERSION
		COREBOOT_EXTRA_VERSION
		" "
		COREBOOT_BUILD
		" starting... (console_loglevel=%d)\n";

	printk(BIOS_ALWAYS, console_test, console_loglevel());
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
 *  For now, for lack of a better alternative,
 *  we will call console_tx_byte ten times and then halt.
 *  Some CPU JTAG debbuggers might have problems but it is the right thing
 *  to do. We call with a NULL since it will clear any FIFOs in the path and
 *  won't clutter up the output, since NULL doesn't print a visible character
 *  on most terminal emulators.
 *
 *  @param str A string to print for the error
 *
 */
void die(const char *str)
{
	int i;

	printk(BIOS_EMERG, str);

	while (1) {
		for (i = 0; i < 10; i++)
			console_tx_byte(0, (void *)0);
		hlt();
	}
}
