/*
 * Bootstrap code for the INTEL 
 */

#include <arch/io.h>
#include <console/console.h>
#include <string.h>
#include <pc80/mc146818rtc.h>


static int initialized;

/* initialize the console */
void console_init(void)
{
	struct console_driver *driver;
	if(get_option(&console_loglevel, "debug_level"))
		console_loglevel=DEFAULT_CONSOLE_LOGLEVEL;
	
	for(driver = console_drivers; driver < econsole_drivers; driver++) {
		if (!driver->init)
			continue;
		driver->init();
	}
	initialized = 1;
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
	if (!initialized)
		return;
	if (byte == '\n')
		__console_tx_byte('\r');
	__console_tx_byte(byte);
}

unsigned char console_rx_byte(void)
{
	struct console_driver *driver;
	if (!initialized)
		return 0;
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
	if (!initialized)
		return 0;
	for(driver = console_drivers; driver < econsole_drivers; driver++)
		if (driver->tst_byte)
			return driver->tst_byte();
	return 0;
}

/*
 *    Write POST information
 */
void post_code(uint8_t value)
{
#if NO_POST==0
#if CONFIG_SERIAL_POST==1
	printk_emerg("POST: 0x%02x\n", value);
#endif
	outb(value, 0x80);
#endif
}

/* Report a fatal error */
void die(const char *msg)
{
	printk_emerg("%s", msg);
	post_code(0xff);
	while (1);		/* Halt */
}
