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

#ifndef CONSOLE_CONSOLE_H_
#define CONSOLE_CONSOLE_H_

#include <stdint.h>
#include <console/loglevel.h>

#ifndef __PRE_RAM__
void console_tx_byte(unsigned char byte);
void console_tx_flush(void);
unsigned char console_rx_byte(void);
int console_tst_byte(void);
#if CONFIG_USBDEBUG
#include <usbdebug.h>
#endif

struct console_driver {
	void (*init)(void);
	void (*tx_byte)(unsigned char byte);
	void (*tx_flush)(void);
	unsigned char (*rx_byte)(void);
	int (*tst_byte)(void);
};

#define __console	__attribute__((used, __section__ (".rodata.console_drivers")))

/* Defined by the linker... */
extern struct console_driver console_drivers[];
extern struct console_driver econsole_drivers[];

extern int console_loglevel;
#else
/* __PRE_RAM__ */
/* Using a global varible can cause problems when we reset the stack
 * from cache as ram to ram. If we make this a define USE_SHARED_STACK
 * we could use the same code on all architectures.
 */
#define console_loglevel CONFIG_DEFAULT_CONSOLE_LOGLEVEL
#if CONFIG_CONSOLE_SERIAL8250
#include <uart8250.h>
#endif
#endif

#ifndef __ROMCC__
void console_init(void);
void post_code(u8 value);
void __attribute__ ((noreturn)) die(const char *msg);
int do_printk(int msg_level, const char *fmt, ...) __attribute__((format(printf, 2, 3)));

#undef WE_CLEANED_UP_ALL_SIDE_EFFECTS
/* We saw some strange effects in the past like coreboot crashing while
 * disabling cache as ram for a maximum console log level of 6 and above while
 * it worked fine without. In order to catch such issues reliably we are
 * always doing a function call to do_printk with the full number of arguments.
 * Our favorite reason to do it this way was:
 *   disable_car();
 *   printk(BIOS_DEBUG, "CAR disabled\n"); // oops, garbage stack pointer
 *   move_stack();
 * This slightly increases the code size and some unprinted strings will end
 * up in the final coreboot binary (most of them compressed). If you want to
 * avoid this, do a
 * #define WE_CLEANED_UP_ALL_SIDE_EFFECTS
 */
#ifdef WE_CLEANED_UP_ALL_SIDE_EFFECTS

#define printk(LEVEL, fmt, args...)				\
	do {							\
		if (CONFIG_MAXIMUM_CONSOLE_LOGLEVEL >= LEVEL) {	\
			do_printk(LEVEL, fmt, ##args);		\
		}						\
	} while(0)

#else

#define printk(LEVEL, fmt, args...)				\
	do {							\
		if (CONFIG_MAXIMUM_CONSOLE_LOGLEVEL >= LEVEL) {	\
			do_printk(LEVEL, fmt, ##args);		\
		} else {					\
			do_printk(BIOS_NEVER, fmt, ##args);	\
		}						\
	} while(0)
#endif

#define print_emerg(STR)         printk(BIOS_EMERG,  "%s", (STR))
#define print_alert(STR)         printk(BIOS_ALERT,  "%s", (STR))
#define print_crit(STR)          printk(BIOS_CRIT,   "%s", (STR))
#define print_err(STR)           printk(BIOS_ERR,    "%s", (STR))
#define print_warning(STR)       printk(BIOS_WARNING,"%s", (STR))
#define print_notice(STR)        printk(BIOS_NOTICE, "%s", (STR))
#define print_info(STR)          printk(BIOS_INFO,   "%s", (STR))
#define print_debug(STR)         printk(BIOS_DEBUG,  "%s", (STR))
#define print_spew(STR)          printk(BIOS_SPEW,   "%s", (STR))

#define print_emerg_char(CH)     printk(BIOS_EMERG,  "%c", (CH))
#define print_alert_char(CH)     printk(BIOS_ALERT,  "%c", (CH))
#define print_crit_char(CH)      printk(BIOS_CRIT,   "%c", (CH))
#define print_err_char(CH)       printk(BIOS_ERR,    "%c", (CH))
#define print_warning_char(CH)   printk(BIOS_WARNING,"%c", (CH))
#define print_notice_char(CH)    printk(BIOS_NOTICE, "%c", (CH))
#define print_info_char(CH)      printk(BIOS_INFO,   "%c", (CH))
#define print_debug_char(CH)     printk(BIOS_DEBUG,  "%c", (CH))
#define print_spew_char(CH)      printk(BIOS_SPEW,   "%c", (CH))

#define print_emerg_hex8(HEX)    printk(BIOS_EMERG,  "%02x",  (HEX))
#define print_alert_hex8(HEX)    printk(BIOS_ALERT,  "%02x",  (HEX))
#define print_crit_hex8(HEX)     printk(BIOS_CRIT,   "%02x",  (HEX))
#define print_err_hex8(HEX)      printk(BIOS_ERR,    "%02x",  (HEX))
#define print_warning_hex8(HEX)  printk(BIOS_WARNING,"%02x",  (HEX))
#define print_notice_hex8(HEX)   printk(BIOS_NOTICE, "%02x",  (HEX))
#define print_info_hex8(HEX)     printk(BIOS_INFO,   "%02x",  (HEX))
#define print_debug_hex8(HEX)    printk(BIOS_DEBUG,  "%02x",  (HEX))
#define print_spew_hex8(HEX)     printk(BIOS_SPEW,   "%02x",  (HEX))

#define print_emerg_hex16(HEX)   printk(BIOS_EMERG,  "%04x", (HEX))
#define print_alert_hex16(HEX)   printk(BIOS_ALERT,  "%04x", (HEX))
#define print_crit_hex16(HEX)    printk(BIOS_CRIT,   "%04x", (HEX))
#define print_err_hex16(HEX)     printk(BIOS_ERR,    "%04x", (HEX))
#define print_warning_hex16(HEX) printk(BIOS_WARNING,"%04x", (HEX))
#define print_notice_hex16(HEX)  printk(BIOS_NOTICE, "%04x", (HEX))
#define print_info_hex16(HEX)    printk(BIOS_INFO,   "%04x", (HEX))
#define print_debug_hex16(HEX)   printk(BIOS_DEBUG,  "%04x", (HEX))
#define print_spew_hex16(HEX)    printk(BIOS_SPEW,   "%04x", (HEX))

#define print_emerg_hex32(HEX)   printk(BIOS_EMERG,  "%08x", (HEX))
#define print_alert_hex32(HEX)   printk(BIOS_ALERT,  "%08x", (HEX))
#define print_crit_hex32(HEX)    printk(BIOS_CRIT,   "%08x", (HEX))
#define print_err_hex32(HEX)     printk(BIOS_ERR,    "%08x", (HEX))
#define print_warning_hex32(HEX) printk(BIOS_WARNING,"%08x", (HEX))
#define print_notice_hex32(HEX)  printk(BIOS_NOTICE, "%08x", (HEX))
#define print_info_hex32(HEX)    printk(BIOS_INFO,   "%08x", (HEX))
#define print_debug_hex32(HEX)   printk(BIOS_DEBUG,  "%08x", (HEX))
#define print_spew_hex32(HEX)    printk(BIOS_SPEW,   "%08x", (HEX))
#else

/* __ROMCC__ */

#if CONFIG_CONSOLE_SERIAL8250
#include "lib/uart8250.c"
#endif
#if CONFIG_CONSOLE_NE2K
#include "lib/ne2k.c"
#endif

static void __console_tx_byte(unsigned char byte)
{
#if CONFIG_CONSOLE_SERIAL8250
	uart8250_tx_byte(CONFIG_TTYS0_BASE, byte);
#endif
#if CONFIG_CONSOLE_NE2K
	ne2k_append_data_byte(byte, CONFIG_CONSOLE_NE2K_IO_PORT);
#endif
}

static void __console_tx_nibble(unsigned nibble)
{
	unsigned char digit;
	digit = nibble + '0';
	if (digit > '9') {
		digit += 39;
	}
	__console_tx_byte(digit);
}

static void __console_tx_char(int loglevel, unsigned char byte)
{
	if (console_loglevel >= loglevel) {
#if CONFIG_CONSOLE_SERIAL8250
		uart8250_tx_byte(CONFIG_TTYS0_BASE, byte);
#endif
#if CONFIG_CONSOLE_NE2K
		ne2k_append_data_byte(byte, CONFIG_CONSOLE_NE2K_IO_PORT);
		ne2k_transmit(CONFIG_CONSOLE_NE2K_IO_PORT);
#endif
	}
}

static void __console_tx_hex8(int loglevel, unsigned char value)
{
	if (console_loglevel >= loglevel) {
		__console_tx_nibble((value >>  4U) & 0x0fU);
		__console_tx_nibble(value & 0x0fU);
	}
#if CONFIG_CONSOLE_NE2K
		ne2k_transmit(CONFIG_CONSOLE_NE2K_IO_PORT);
#endif
}

static void __console_tx_hex16(int loglevel, unsigned short value)
{
	if (console_loglevel >= loglevel) {
		__console_tx_nibble((value >> 12U) & 0x0fU);
		__console_tx_nibble((value >>  8U) & 0x0fU);
		__console_tx_nibble((value >>  4U) & 0x0fU);
		__console_tx_nibble(value & 0x0fU);
	}
#if CONFIG_CONSOLE_NE2K
		ne2k_transmit(CONFIG_CONSOLE_NE2K_IO_PORT);
#endif
}

static void __console_tx_hex32(int loglevel, unsigned int value)
{
	if (console_loglevel >= loglevel) {
		__console_tx_nibble((value >> 28U) & 0x0fU);
		__console_tx_nibble((value >> 24U) & 0x0fU);
		__console_tx_nibble((value >> 20U) & 0x0fU);
		__console_tx_nibble((value >> 16U) & 0x0fU);
		__console_tx_nibble((value >> 12U) & 0x0fU);
		__console_tx_nibble((value >>  8U) & 0x0fU);
		__console_tx_nibble((value >>  4U) & 0x0fU);
		__console_tx_nibble(value & 0x0fU);
	}
#if CONFIG_CONSOLE_NE2K
		ne2k_transmit(CONFIG_CONSOLE_NE2K_IO_PORT);
#endif
}

static void __console_tx_string(int loglevel, const char *str)
{
	if (console_loglevel >= loglevel) {
		unsigned char ch;
		while((ch = *str++) != '\0') {
			if (ch == '\n')
				__console_tx_byte('\r');
			__console_tx_byte(ch);
		}
#if CONFIG_CONSOLE_NE2K
		ne2k_transmit(CONFIG_CONSOLE_NE2K_IO_PORT);
#endif
	}
}

#define FUNCTIONS_FOR_PRINT
#ifdef  FUNCTIONS_FOR_PRINT
static void print_emerg_char(unsigned char byte) { __console_tx_char(BIOS_EMERG, byte); }
static void print_emerg_hex8(unsigned char value){ __console_tx_hex8(BIOS_EMERG, value); }
static void print_emerg_hex16(unsigned short value){ __console_tx_hex16(BIOS_EMERG, value); }
static void print_emerg_hex32(unsigned int value) { __console_tx_hex32(BIOS_EMERG, value); }
static void print_emerg(const char *str) { __console_tx_string(BIOS_EMERG, str); }

static void print_alert_char(unsigned char byte) { __console_tx_char(BIOS_ALERT, byte); }
static void print_alert_hex8(unsigned char value) { __console_tx_hex8(BIOS_ALERT, value); }
static void print_alert_hex16(unsigned short value){ __console_tx_hex16(BIOS_ALERT, value); }
static void print_alert_hex32(unsigned int value) { __console_tx_hex32(BIOS_ALERT, value); }
static void print_alert(const char *str) { __console_tx_string(BIOS_ALERT, str); }

static void print_crit_char(unsigned char byte) { __console_tx_char(BIOS_CRIT, byte); }
static void print_crit_hex8(unsigned char value) { __console_tx_hex8(BIOS_CRIT, value); }
static void print_crit_hex16(unsigned short value){ __console_tx_hex16(BIOS_CRIT, value); }
static void print_crit_hex32(unsigned int value) { __console_tx_hex32(BIOS_CRIT, value); }
static void print_crit(const char *str) { __console_tx_string(BIOS_CRIT, str); }

static void print_err_char(unsigned char byte) { __console_tx_char(BIOS_ERR, byte); }
static void print_err_hex8(unsigned char value) { __console_tx_hex8(BIOS_ERR, value); }
static void print_err_hex16(unsigned short value){ __console_tx_hex16(BIOS_ERR, value); }
static void print_err_hex32(unsigned int value) { __console_tx_hex32(BIOS_ERR, value); }
static void print_err(const char *str) { __console_tx_string(BIOS_ERR, str); }

static void print_warning_char(unsigned char byte) { __console_tx_char(BIOS_WARNING, byte); }
static void print_warning_hex8(unsigned char value) { __console_tx_hex8(BIOS_WARNING, value); }
static void print_warning_hex16(unsigned short value){ __console_tx_hex16(BIOS_WARNING, value); }
static void print_warning_hex32(unsigned int value) { __console_tx_hex32(BIOS_WARNING, value); }
static void print_warning(const char *str) { __console_tx_string(BIOS_WARNING, str); }

static void print_notice_char(unsigned char byte) { __console_tx_char(BIOS_NOTICE, byte); }
static void print_notice_hex8(unsigned char value) { __console_tx_hex8(BIOS_NOTICE, value); }
static void print_notice_hex16(unsigned short value){ __console_tx_hex16(BIOS_NOTICE, value); }
static void print_notice_hex32(unsigned int value) { __console_tx_hex32(BIOS_NOTICE, value); }
static void print_notice(const char *str) { __console_tx_string(BIOS_NOTICE, str); }

static void print_info_char(unsigned char byte) { __console_tx_char(BIOS_INFO, byte); }
static void print_info_hex8(unsigned char value) { __console_tx_hex8(BIOS_INFO, value); }
static void print_info_hex16(unsigned short value){ __console_tx_hex16(BIOS_INFO, value); }
static void print_info_hex32(unsigned int value) { __console_tx_hex32(BIOS_INFO, value); }
static void print_info(const char *str) { __console_tx_string(BIOS_INFO, str); }

static void print_debug_char(unsigned char byte) { __console_tx_char(BIOS_DEBUG, byte); }
static void print_debug_hex8(unsigned char value) { __console_tx_hex8(BIOS_DEBUG, value); }
static void print_debug_hex16(unsigned short value){ __console_tx_hex16(BIOS_DEBUG, value); }
static void print_debug_hex32(unsigned int value) { __console_tx_hex32(BIOS_DEBUG, value); }
static void print_debug(const char *str) { __console_tx_string(BIOS_DEBUG, str); }

static void print_spew_char(unsigned char byte) { __console_tx_char(BIOS_SPEW, byte); }
static void print_spew_hex8(unsigned char value) { __console_tx_hex8(BIOS_SPEW, value); }
static void print_spew_hex16(unsigned short value){ __console_tx_hex16(BIOS_SPEW, value); }
static void print_spew_hex32(unsigned int value) { __console_tx_hex32(BIOS_SPEW, value); }
static void print_spew(const char *str) { __console_tx_string(BIOS_SPEW, str); }

#else
#define print_emerg(STR)         __console_tx_string(BIOS_EMERG, STR)
#define print_alert(STR)         __console_tx_string(BIOS_ALERT, STR)
#define print_crit(STR)          __console_tx_string(BIOS_CRIT, STR)
#define print_err(STR)           __console_tx_string(BIOS_ERR, STR)
#define print_warning(STR)       __console_tx_string(BIOS_WARNING, STR)
#define print_notice(STR)        __console_tx_string(BIOS_NOTICE, STR)
#define print_info(STR)          __console_tx_string(BIOS_INFO, STR)
#define print_debug(STR)         __console_tx_string(BIOS_DEBUG, STR)
#define print_spew(STR)          __console_tx_string(BIOS_SPEW, STR)

#define print_emerg_char(CH)     __console_tx_char(BIOS_EMERG, CH)
#define print_alert_char(CH)     __console_tx_char(BIOS_ALERT, CH)
#define print_crit_char(CH)      __console_tx_char(BIOS_CRIT, CH)
#define print_err_char(CH)       __console_tx_char(BIOS_ERR, CH)
#define print_warning_char(CH)   __console_tx_char(BIOS_WARNING, CH)
#define print_notice_char(CH)    __console_tx_char(BIOS_NOTICE, CH)
#define print_info_char(CH)      __console_tx_char(BIOS_INFO, CH)
#define print_debug_char(CH)     __console_tx_char(BIOS_DEBUG, CH)
#define print_spew_char(CH)      __console_tx_char(BIOS_SPEW, CH)

#define print_emerg_hex8(HEX)    __console_tx_hex8(BIOS_EMERG, HEX)
#define print_alert_hex8(HEX)    __console_tx_hex8(BIOS_ALERT, HEX)
#define print_crit_hex8(HEX)     __console_tx_hex8(BIOS_CRIT, HEX)
#define print_err_hex8(HEX)      __console_tx_hex8(BIOS_ERR, HEX)
#define print_warning_hex8(HEX)  __console_tx_hex8(BIOS_WARNING, HEX)
#define print_notice_hex8(HEX)   __console_tx_hex8(BIOS_NOTICE, HEX)
#define print_info_hex8(HEX)     __console_tx_hex8(BIOS_INFO, HEX)
#define print_debug_hex8(HEX)    __console_tx_hex8(BIOS_DEBUG, HEX)
#define print_spew_hex8(HEX)     __console_tx_hex8(BIOS_SPEW, HEX)

#define print_emerg_hex16(HEX)   __console_tx_hex16(BIOS_EMERG, HEX)
#define print_alert_hex16(HEX)   __console_tx_hex16(BIOS_ALERT, HEX)
#define print_crit_hex16(HEX)    __console_tx_hex16(BIOS_CRIT, HEX)
#define print_err_hex16(HEX)     __console_tx_hex16(BIOS_ERR, HEX)
#define print_warning_hex16(HEX) __console_tx_hex16(BIOS_WARNING, HEX)
#define print_notice_hex16(HEX)  __console_tx_hex16(BIOS_NOTICE, HEX)
#define print_info_hex16(HEX)    __console_tx_hex16(BIOS_INFO, HEX)
#define print_debug_hex16(HEX)   __console_tx_hex16(BIOS_DEBUG, HEX)
#define print_spew_hex16(HEX)    __console_tx_hex16(BIOS_SPEW, HEX)

#define print_emerg_hex32(HEX)   __console_tx_hex32(BIOS_EMERG, HEX)
#define print_alert_hex32(HEX)   __console_tx_hex32(BIOS_ALERT, HEX)
#define print_crit_hex32(HEX)    __console_tx_hex32(BIOS_CRIT, HEX)
#define print_err_hex32(HEX)     __console_tx_hex32(BIOS_ERR, HEX)
#define print_warning_hex32(HEX) __console_tx_hex32(BIOS_WARNING, HEX)
#define print_notice_hex32(HEX)  __console_tx_hex32(BIOS_NOTICE, HEX)
#define print_info_hex32(HEX)    __console_tx_hex32(BIOS_INFO, HEX)
#define print_debug_hex32(HEX)   __console_tx_hex32(BIOS_DEBUG, HEX)
#define print_spew_hex32(HEX)    __console_tx_hex32(BIOS_SPEW, HEX)
#endif

#endif

#ifdef __ROMCC__
/* if included by romcc, include the sources, too. romcc can't use prototypes */
#include <console/console.c>
#include <console/post.c>
#include <console/die.c>
#endif

#endif /* CONSOLE_CONSOLE_H_ */
