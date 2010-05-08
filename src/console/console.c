/*
 * Bootstrap code for the INTEL
 */

#include <console/console.h>
#include <build.h>
#include <arch/hlt.h>
#include <arch/io.h>

#ifndef __PRE_RAM__
#include <string.h>
#include <pc80/mc146818rtc.h>


/* initialize the console */
void console_init(void)
{
	struct console_driver *driver;
	if(get_option(&console_loglevel, "debug_level"))
		console_loglevel=CONFIG_DEFAULT_CONSOLE_LOGLEVEL;

	for(driver = console_drivers; driver < econsole_drivers; driver++) {
		if (!driver->init)
			continue;
		driver->init();
	}
}

static void __console_tx_byte(unsigned char byte)
{
	struct console_driver *driver;
	for(driver = console_drivers; driver < econsole_drivers; driver++) {
		driver->tx_byte(byte);
	}
}

void console_tx_flush(void)
{
	struct console_driver *driver;
	for(driver = console_drivers; driver < econsole_drivers; driver++) {
		if (!driver->tx_flush)
			continue;
		driver->tx_flush();
	}
}

void console_tx_byte(unsigned char byte)
{
	if (byte == '\n')
		__console_tx_byte('\r');
	__console_tx_byte(byte);
}

unsigned char console_rx_byte(void)
{
	struct console_driver *driver;
	for(driver = console_drivers; driver < econsole_drivers; driver++) {
		if (driver->tst_byte)
			break;
	}
	if (driver == econsole_drivers)
		return 0;
	while (!driver->tst_byte());
	return driver->rx_byte();
}

int console_tst_byte(void)
{
	struct console_driver *driver;
	for(driver = console_drivers; driver < econsole_drivers; driver++)
		if (driver->tst_byte)
			return driver->tst_byte();
	return 0;
}

/*
 *    Write POST information
 */
void post_code(u8 value)
{
#if !defined(CONFIG_NO_POST) || CONFIG_NO_POST==0
#if CONFIG_SERIAL_POST==1
	printk(BIOS_EMERG, "POST: 0x%02x\n", value);
#endif
	outb(value, 0x80);
#endif
}

/* Report a fatal error */
void __attribute__((noreturn)) die(const char *msg)
{
	printk(BIOS_EMERG, "%s", msg);
	//post_code(0xff);
 	for (;;)
		hlt();		/* Halt */
}

#else

void console_init(void)
{
	static const char console_test[] =
		"\n\ncoreboot-"
		COREBOOT_VERSION
		COREBOOT_EXTRA_VERSION
		" "
		COREBOOT_BUILD
		" starting...\n";
	print_info(console_test);
}

void post_code(u8 value)
{
#if !defined(CONFIG_NO_POST) || CONFIG_NO_POST==0
#if CONFIG_SERIAL_POST==1
	print_emerg("POST: 0x");
	print_emerg_hex8(value);
	print_emerg("\n");
#endif
	outb(value, 0x80);
#endif
}

void die(const char *str)
{
	print_emerg(str);
	do {
		hlt();
	} while(1);
}
#endif
