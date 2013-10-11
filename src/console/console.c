/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Eric Biederman
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <build.h>
#include <arch/hlt.h>
#include <arch/io.h>

#ifndef __PRE_RAM__
#include <string.h>

/*
 * FIXME: get_option() needs to be abstracted better so that other non-volatile
 * storage can be used. This will benefit machines without CMOS as well as those
 * without a battery-backed CMOS (e.g. some laptops).
 */
#if CONFIG_USE_OPTION_TABLE
#include <pc80/mc146818rtc.h>
#else
static inline int get_option(void *dest, const char *name) { return -1; }
#endif

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

void console_tx_flush(void)
{
	struct console_driver *driver;
	for(driver = console_drivers; driver < econsole_drivers; driver++) {
		if (!driver->tx_flush)
			continue;
		driver->tx_flush();
	}
}

static void __console_tx_byte(unsigned char byte)
{
	struct console_driver *driver;
	for(driver = console_drivers; driver < econsole_drivers; driver++) {
		driver->tx_byte(byte);
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

#else // __PRE_RAM__   ^^^ NOT defined   vvv defined

void console_init(void)
{
#if defined(__BOOT_BLOCK__) && CONFIG_BOOTBLOCK_CONSOLE || \
    !defined(__BOOT_BLOCK__) && CONFIG_EARLY_CONSOLE

#if CONFIG_CONSOLE_SERIAL
	uart_init();
#endif
#if CONFIG_DRIVERS_OXFORD_OXPCIE && CONFIG_CONSOLE_SERIAL8250MEM
	oxford_init();
#endif
#if CONFIG_CONSOLE_NE2K
	ne2k_init(CONFIG_CONSOLE_NE2K_IO_PORT);
#endif
#if CONFIG_CONSOLE_CBMEM && CONFIG_EARLY_CBMEM_INIT && !defined(__BOOT_BLOCK__)
	cbmemc_init();
#endif
#if CONFIG_SPKMODEM
	spkmodem_init();
#endif
#if CONFIG_USBDEBUG_IN_ROMSTAGE && !defined(__BOOT_BLOCK__)
	usbdebug_init();
#endif

	static const char console_test[] =
		"\n\ncoreboot-"
		COREBOOT_VERSION
		COREBOOT_EXTRA_VERSION
		" "
		COREBOOT_BUILD
		" starting...\n";
	print_info(console_test);

#endif /* CONFIG_EARLY_CONSOLE */
}
#endif
